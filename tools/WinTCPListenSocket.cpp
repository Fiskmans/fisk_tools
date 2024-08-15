
#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_WINDOWS

#include "tools/Logger.h"
#include "tools/TCPListenSocket.h"
#include "tools/Trace.h"

#include <cassert>

#define NOMINMAX
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

namespace fisk::tools
{
	TCPListenSocket::TCPListenSocket(Port aPort)
	{
		mySocket.myValue = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

		if (mySocket.myValue == INVALID_SOCKET)
		{
			LOG_SYS_ERROR("Failed to create tcp listen socket", 
				"Port: " + std::to_string(aPort), 
				"Error: " + std::to_string(::WSAGetLastError()));
			return;
		}

		DWORD ipv6Only = FALSE;

		int dualStackResult = ::setsockopt(mySocket.myValue, IPPROTO_IPV6, IPV6_V6ONLY,
										   reinterpret_cast<const char*>(&ipv6Only), sizeof(ipv6Only));

		if (dualStackResult != 0)
		{
			LOG_SYS_WARNING("Failed to dual-stack tcp listen socket, it will continue operating in IPv6 only mode",
							"Port: " + std::to_string(aPort), "Error: " + std::to_string(::WSAGetLastError()));
		}

		u_long mode = 1;
		int nonBlockingResult = ::ioctlsocket(mySocket.myValue, FIONBIO, &mode);

		if (nonBlockingResult != 0)
		{
			LOG_SYS_WARNING("Failed to make listensocket nonBlocking",
							"Port: " + std::to_string(aPort), "Error: " + std::to_string(::WSAGetLastError()));
			return;
		}


		sockaddr_in6 localAddr;
		localAddr.sin6_family	= AF_INET6;
		localAddr.sin6_addr		= IN6ADDR_ANY_INIT;
		localAddr.sin6_port		= htons(aPort);
		localAddr.sin6_scope_id = 0;

		int bindResult = ::bind(mySocket.myValue, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr));
		if (bindResult != 0)
		{
			LOG_SYS_ERROR("Failed to bind tcp listen socket", 
				"Port: " + std::to_string(aPort), 
				"Error: " + std::to_string(::WSAGetLastError()));
			return;
		}

		if (aPort == AnyPort)
		{
			sockaddr_in6 addr;
			memset(&addr, 0, sizeof(addr));

			int len			   = sizeof(addr);
			int sockNameResult = ::getsockname(mySocket.myValue, reinterpret_cast<sockaddr*>(&addr), &len);
			if (sockNameResult == 0)
			{
				if (len == sizeof(addr))
				{
					myPort = htons(addr.sin6_port);
				}
				else if (len == sizeof(sockaddr_in))
				{
					myPort = htons(reinterpret_cast<sockaddr_in*>(&addr)->sin_port);
				}
				else
				{
					LOG_SYS_WARNING("Unkown sockaddr type when getting socket name, it will continue to function but "
									"the port reported will be wrong");
				}
			}
			else
			{
				LOG_SYS_WARNING("Failed getsockname after binding tcp listen socket, it will continue to function but the port reported will be wrong", 
					"Error: " + std::to_string(::WSAGetLastError()));
			}
		}
		else
		{
			myPort = aPort;
		}

		int listenResult = ::listen(mySocket.myValue, SOMAXCONN);
		if (listenResult != 0)
		{
			LOG_SYS_ERROR("Failed to listen with tcp listen socket", 
				"Port: " + std::to_string(aPort), 
				"Error: " + std::to_string(::WSAGetLastError()));
			return;
		}
	}

	bool TCPListenSocket::Update()
	{
		FISK_TRACE("tcp_listen_update");
		while (true)
		{
			FISK_TRACE("accept_attempt");
			SOCKET next = ::accept(mySocket.myValue, nullptr, nullptr);

			if (next == INVALID_SOCKET)
			{
				int err = ::WSAGetLastError();
				switch (err)
				{
				case WSANOTINITIALISED:
				case WSAEFAULT:
				case WSAEINVAL:
				case WSAEINPROGRESS:
				case WSAENOTSOCK:
				case WSAEOPNOTSUPP:
					assert(false);
					return false;

				case WSAECONNRESET:
				case WSAEINTR:
				case WSAEMFILE:
				case WSAENETDOWN:
					return false;

				case WSAENOBUFS:
				case WSAEWOULDBLOCK:
					return true;
				}
			}

			{
				FISK_TRACE("accept_success_event");
				std::shared_ptr<Socket> sock = std::make_shared<Socket>();
				sock->myValue				 = next;

				OnNewConnection.Fire(std::make_shared<TCPSocket>(sock));
			}
		}
	}

} // namespace fisk::tools

#endif