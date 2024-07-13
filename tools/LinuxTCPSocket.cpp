#ifndef FISK_TOOLS_TCP_SOCKET_WIN_H
#define FISK_TOOLS_TCP_SOCKET_WIN_H

#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_LINUX

#include "tools/TCPSocket.h"

#include <sys/ioctl.h>

namespace fisk::tools
{
	TCPSocket::TCPSocket(std::shared_ptr<Socket> aSocket)
		: mySocket(aSocket)
	{
		if (mySocket)
		{
			u_long mode = 1; 
			::ioctl(mySocket->myValue, FIONBIO, &mode);
		}
	}

	TCPSocket::TCPSocket(const char* aName, const char* aServiceOrPort, std::chrono::microseconds aTimeout)
	{
		mySocket = std::make_shared<Socket>();


		addrinfo hints = {};
		addrinfo* addresses;

		hints.ai_family		= AF_UNSPEC;
		hints.ai_socktype	= SOCK_STREAM;
		hints.ai_protocol	= IPPROTO_TCP;

		if (::getaddrinfo(aName, aServiceOrPort, &hints, addresses) != 0)
			assert(false);

		addrinfo* at = addresses;
		while (at)
		{
			mySocket->myValue = ::socket(at->ai_family, at->ai_socktype, at->ai_protocol);
			if (mySocket->myValue == -1)
			{
				at = at->ai_next;
				continue;
			}

			if (::connect(mySocket->myValue, at->ai_addr, at->ai_addrlen) != 0)
			{
				::close(mySocket->myValue);
				mySocket->myValue = -1;

				at = at->ai_next;
				continue;
			}

			break;
		}

		if (mySocket->myValue == -1)
			assert(false);

		u_long mode = 1;
		::ioctlsocket(mySocket->myValue, FIONBIO, &mode);
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

	void TCPSocket::Close()
	{
		mySocket.reset();
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

} // namespace fisk::tools

#endif
#endif
