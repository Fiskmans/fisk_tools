#ifndef FISK_TOOLS_TCP_SOCKET_WIN_H
#define FISK_TOOLS_TCP_SOCKET_WIN_H

#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_WINDOWS

#include "tools/TCPSocket.h"

#define NOMINMAX
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

namespace fisk::tools
{
	TCPSocket::TCPSocket(std::shared_ptr<Socket> aSocket)
		: mySocket(aSocket)
	{
		if (mySocket)
		{
			u_long mode = 1; 
			::ioctlsocket(mySocket->myValue, FIONBIO, &mode);
		}
	}

	bool TCPSocket::Update()
	{
		if (!mySocket)
			return false;

		if (!HandleSend())
			return false;

		if (!HandleRecv())
			return false;

		return true;
	}

	bool TCPSocket::HandleSend()
	{
		while (myWriteStream.HasData() || myWriteQueue)
		{
			if (!myWriteQueue)
			{
				myWriteQueue.mySegment = myWriteStream.Get();
				myWriteQueue.myOffset  = 0;
			}

			int amount = ::send(mySocket->myValue,
								reinterpret_cast<const char*>(myWriteQueue.mySegment->myData + myWriteQueue.myOffset),
								myWriteQueue.mySegment->mySize - myWriteQueue.myOffset, 0);

			if (amount == SOCKET_ERROR)
			{
				int err = ::WSAGetLastError();
				switch (err)
				{
				case WSANOTINITIALISED:
				case WSAEACCES:
				case WSAEFAULT:
				case WSAENOTCONN:
				case WSAENOTSOCK:
				case WSAEOPNOTSUPP:
				case WSAESHUTDOWN:
				case WSAEMSGSIZE:
				case WSAEINVAL:
					assert(false);
					return false;

				case WSAENETDOWN:
				case WSAENETRESET:
				case WSAEHOSTUNREACH:
				case WSAECONNABORTED:
				case WSAECONNRESET:
				case WSAETIMEDOUT:
					return false;

				case WSAEWOULDBLOCK:
				case WSAENOBUFS:
				case WSAEINTR:
				case WSAEINPROGRESS:
					return true;
				}
			}

			myWriteQueue.myOffset += amount;
			if (myWriteQueue.myOffset == myWriteQueue.mySegment->mySize)
			{
				myWriteQueue.mySegment = myWriteQueue.mySegment->myNext;
				myWriteQueue.myOffset  = 0;
			}
			else
			{
				break;
			}
		}

		return true;
	}

	bool TCPSocket::HandleRecv()
	{
		bool readData = false;
		while (true)
		{
			if (!myReadSegment)
				myReadSegment = std::make_shared<StreamSegment>();

			int amount = ::recv(mySocket->myValue, reinterpret_cast<char*>(myReadSegment->myData), StreamSegment::CHUNK_SIZE, 0);

			if (amount == 0)
				return false;

			if (amount == SOCKET_ERROR)
			{
				int err = ::WSAGetLastError();
				switch (err)
				{
				case WSANOTINITIALISED:
				case WSAEFAULT:
				case WSAENOTCONN:
				case WSAENOTSOCK:
				case WSAEOPNOTSUPP:
				case WSAESHUTDOWN:
				case WSAEMSGSIZE:
				case WSAEINVAL:
					assert(false);
					return false;

				case WSAENETDOWN:
				case WSAENETRESET:
				case WSAECONNABORTED:
				case WSAETIMEDOUT:
				case WSAECONNRESET:
					return false;

				case WSAEINTR:
				case WSAEINPROGRESS:
				case WSAEWOULDBLOCK:
					if (readData)
						OnDataAvailable.Fire();

					return true;
				}
			}

			readData = true;
			myReadSegment->mySize = amount;
			myReadStream.AppendData(myReadSegment);
			myReadSegment.reset();
		}

		return true;
	}

	TCPSocket ConnectToTCPByName(const char* aName, const char* aServiceOrPort, std::chrono::microseconds aTimeout)
	{
		std::shared_ptr<Socket> sock = std::make_shared<Socket>();
		sock->myValue				 = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		sockaddr_in6 localAddress;
		DWORD localAddressSize = sizeof(localAddress);

		sockaddr_in6 remoteAddress;
		DWORD remoteAddressSize = sizeof(remoteAddress);

		
		std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(aTimeout);
		std::chrono::microseconds micros = aTimeout - std::chrono::duration_cast<std::chrono::microseconds>(seconds);

		timeval timeout;
		timeout.tv_sec = seconds.count();
		timeout.tv_usec = micros.count();

		BOOL result = ::WSAConnectByNameA(sock->myValue, aName, aServiceOrPort, &localAddressSize,
										  reinterpret_cast<sockaddr*>(&localAddress), &remoteAddressSize,
										  reinterpret_cast<sockaddr*>(&remoteAddress), &timeout, NULL);

		if (result == FALSE)
			return TCPSocket(nullptr);

		return TCPSocket(sock);
	}

} // namespace fisk::tools

#endif
#endif
