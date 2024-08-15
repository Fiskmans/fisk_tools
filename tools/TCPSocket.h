#ifndef FISK_TOOLS_TCP_SOCKET_H
#define FISK_TOOLS_TCP_SOCKET_H

#include "tools/Socket.h"
#include "tools/Stream.h"
#include "tools/Event.h"

#include <chrono>

namespace fisk::tools
{
	class TCPSocket
	{
	public:
		TCPSocket(std::shared_ptr<Socket> aSocket);
		TCPSocket(const char* aName, const char* aServiceOrPort, std::chrono::microseconds aTimeout);
		TCPSocket(const TCPSocket&) = delete;
		void operator=(const TCPSocket&) = delete;

		bool Update();
		void Close();

		WriteStream& GetWriteStream();
		ReadStream& GetReadStream();
		const Socket& GetSocket();

		Event<> OnDataAvailable;

	private:

		bool HandleSend();
		bool HandleRecv();

		WriteStream myWriteStream;
		StreamOffset myWriteQueue;

		ReadStream myReadStream;
		std::shared_ptr<StreamSegment> myReadSegment;

		std::shared_ptr<Socket> mySocket;
	};


	inline WriteStream& TCPSocket::GetWriteStream()
	{
		return myWriteStream;
	}

	inline ReadStream& TCPSocket::GetReadStream()
	{
		return myReadStream;
	}

	inline const Socket& TCPSocket::GetSocket()
	{
		return *mySocket;
	}
}

#endif