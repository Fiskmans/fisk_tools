#ifndef FISK_TOOLS_TCP_LISTEN_SOCKET_H
#define FISK_TOOLS_TCP_LISTEN_SOCKET_H

#include "tools/Event.h"
#include "tools/net/Net.h"
#include "tools/net/Socket.h"
#include "tools/net/TCPSocket.h"

#include <limits>

namespace fisk::tools
{
	class TCPListenSocket
	{
	public:
		static constexpr Port AnyPort = 0;

		TCPListenSocket(Port aPort);
		~TCPListenSocket() = default;

		bool Update();
		Port GetPort() const;

		Event<std::shared_ptr<TCPSocket>> OnNewConnection;

	private:
		Port myPort = AnyPort;
		Socket mySocket;
	};

	inline Port TCPListenSocket::GetPort() const
	{
		return myPort;
	}
}

#endif