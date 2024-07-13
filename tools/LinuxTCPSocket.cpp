#ifndef FISK_TOOLS_TCP_SOCKET_WIN_H
#define FISK_TOOLS_TCP_SOCKET_WIN_H

#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_LINUX

#include "tools/TCPSocket.h"

#include <netdb.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>


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
		if (::ioctl(mySocket->myValue, FIONBIO, &mode) != 0)
			assert(false);
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
				int err = errno;
				switch (err)
				{
				case EBADF:
				case EACCES:
				case EDESTADDRREQ:
				case EFAULT:
				case EISCONN:
				case EMSGSIZE:
				case ENOTCONN:
				case EINVAL:
				case ENOTSOCK:
				case EOPNOTSUPP:
					assert(false);
					return false;

				case EALREADY:
				case ENOBUFS:
				case ENOMEM:
				case EMSGSIZE:
				case ECONNRESET:
				case EPIPE:
					return false;

				case EAGAIN:
				case EINTR:
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
				int err = ::errno;
				switch (err)
				{
				case EBADF:
				case EFAULT:
				case EINVAL:
				case ENOTCONN:
				case ENOTSOCK:
					assert(false);
					return false;

				case ENOMEM:
					return false;

				case EAGAIN:
				case EINTR:
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
