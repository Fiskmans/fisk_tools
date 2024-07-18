#include "tools/TCPListenSocket.h"
#include <iostream>

class PrintingSocket
{
public:
	PrintingSocket(std::shared_ptr<fisk::tools::TCPSocket> aBaseSocket)
	{
		mySocket = aBaseSocket;

		myPrintingEvent = mySocket->OnDataAvailable.Register(
			[this]()
			{
				size_t amount;
				do
				{
					uint8_t chunk[512 + 1] = {};
					amount = mySocket->GetReadStream().Peek(chunk, 512);
					mySocket->GetReadStream().Read(chunk, amount);
					chunk[amount] = '\0';


					std::cout << std::string(chunk, chunk + amount) << std::endl;

				} while (amount == 512);
			});
	}

	bool Update()
	{
		return mySocket->Update();
	}

private:
	std::shared_ptr<fisk::tools::TCPSocket> mySocket;
	fisk::tools::EventReg myPrintingEvent;
};

void UpdateSockets(std::vector<PrintingSocket*>& aSockets)
{
	for (int i = aSockets.size() - 1; i >= 0; i--)
	{
		PrintingSocket* socket = aSockets[i];

		if (!socket->Update())
		{
			aSockets.erase(aSockets.begin() + i);
			delete socket;
			std::cout << "Socket closed" << std::endl;
		}
	}
}

int main()
{
	fisk::tools::TCPListenSocket listenSocket(12345);

	std::vector<PrintingSocket*> sockets;

	fisk::tools::EventReg newCons = listenSocket.OnNewConnection.Register(
		[&sockets](std::shared_ptr<fisk::tools::TCPSocket> aSocket)
		{
			std::cout << "new connection" << std::endl;
			sockets.push_back(new PrintingSocket(aSocket));
		});


	std::cout << "Listening" << std::endl;

	while (listenSocket.Update())
	{
		UpdateSockets(sockets);
	}

	std::cout << "listen socket closed" << std::endl;

	while (!sockets.empty())
	{
		UpdateSockets(sockets);
	}

	std::cout << "Last socket closed" << std::endl;
}
