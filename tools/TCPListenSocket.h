#ifndef FISK_TOOLS_TCP_LISTEN_SOCKET_H
#define FISK_TOOLS_TCP_LISTEN_SOCKET_H

#include "tools/Event.h"
#include "tools/Net.h"
#include "tools/Socket.h"
#include "tools/TCPSocket.h"

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
		Port GetPort();

		Event<std::shared_ptr<TCPSocket>> OnNewConnection;

	private:
		Port myPort = AnyPort;
		Socket mySocket;
	};
}

#endif