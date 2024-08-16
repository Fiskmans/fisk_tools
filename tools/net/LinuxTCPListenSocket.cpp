
#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_LINUX

#include "tools/Logger.h"
#include "tools/net/TCPListenSocket.h"

#include <cassert>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

namespace fisk::tools
{
	TCPListenSocket::TCPListenSocket(Port aPort)
	{
		mySocket.myValue = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

		if (mySocket.myValue == -1)
		{
			LOG_SYS_ERROR("Failed to create tcp listen socket", 
				"Port: " + std::to_string(aPort), 
				"Error: " + std::to_string(errno));
			return;
		}

		int ipv6Only = 0;

		int dualStackResult = ::setsockopt(mySocket.myValue, IPPROTO_IPV6, IPV6_V6ONLY,
										   reinterpret_cast<const char*>(&ipv6Only), sizeof(ipv6Only));

		if (dualStackResult != 0)
		{
			LOG_SYS_WARNING("Failed to dual-stack tcp listen socket, it will continue operating in IPv6 only mode",
							"Port: " + std::to_string(aPort), "Error: " + std::to_string(errno));
		}

		u_long mode = 1;
		int nonBlockingResult = ::ioctl(mySocket.myValue, FIONBIO, &mode);

		if (nonBlockingResult != 0)
		{
			LOG_SYS_WARNING("Failed to make listensocket nonBlocking",
				"Port: " + std::to_string(aPort), "Error: " + std::to_string(errno));
			return;
		}


		sockaddr_in6 localAddr;
		localAddr.sin6_family = AF_INET6;
		localAddr.sin6_addr = IN6ADDR_ANY_INIT;
		localAddr.sin6_port = htons(aPort);
		localAddr.sin6_scope_id = 0;

		int bindResult = ::bind(mySocket.myValue, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr));
		if (bindResult != 0)
		{
			LOG_SYS_ERROR("Failed to bind tcp listen socket",
				"Port: " + std::to_string(aPort),
				"Error: " + std::to_string(errno));
			return;
		}

		if (aPort == AnyPort)
		{
			sockaddr_in6 addr;
			memset(&addr, 0, sizeof(addr));

			socklen_t len = sizeof(addr);
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
					"Error: " + std::to_string(errno));
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
				"Error: " + std::to_string(errno));
			return;
		}
	}

	bool TCPListenSocket::Update()
	{
		while (true)
		{
			int next = ::accept(mySocket.myValue, nullptr, nullptr);

			if (next == -1)
			{
				int err = errno;
				switch (err)
				{
				case EBADF:
				case EFAULT:
				case EINVAL:
				case ENOTSOCK:
				case EOPNOTSUPP:
					assert(false);
					return false;

				case ECONNRESET:
				case EINTR:
				case ENOMEM:
				case EPERM:
					return false;

				case ECONNABORTED:
				case EAGAIN:
				case EMFILE:
				case EPROTO:
					return true;
				}
			}

			std::shared_ptr<Socket> sock = std::make_shared<Socket>();
			sock->myValue				 = next;



			OnNewConnection.Fire(std::make_shared<TCPSocket>(sock));
		}
	}

} // namespace fisk::tools

#endif
