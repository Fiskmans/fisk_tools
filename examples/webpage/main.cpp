

#include "tools/http/Server.h"
#include "tools/TCPSocket.h"
#include "tools/TCPListenSocket.h"

#include <iostream>

class FixedEndpoint : public fisk::tools::http::Endpoint
{
public:
	inline FixedEndpoint(std::string aContent)
	{
		myResponse.myCode = fisk::tools::http::CommonResponseCodes::OK;

		myResponse.WriteRawHtml(aContent);
		myResponse.CalculateSize();
	}

	fisk::tools::http::ResponseFrame OnFrame(const fisk::tools::http::RequestFrame& aFrame) override
	{
		std::cout << "Sent" << std::endl;
		return myResponse;
	}

private:
	fisk::tools::http::ResponseFrame myResponse;
};

class Client
{
public:
	inline Client(std::shared_ptr<fisk::tools::TCPSocket> aSocket, fisk::tools::http::Server& aServer)
	{
		mySocket = aSocket;
		myDataHandle = mySocket->OnDataAvailable.Register([this, &aServer]()
		{
			aServer.UpdateOn(mySocket->GetReadStream(), mySocket->GetWriteStream());
		});
	}
	bool Update()
	{
		return mySocket->Update();
	}

private:
	std::shared_ptr<fisk::tools::TCPSocket> mySocket;
	fisk::tools::EventReg myDataHandle;
};

int main()
{
	fisk::tools::http::Server server;

	FixedEndpoint helloWorld(
		"<h1>Hello world!</h1>"
		"<a href=\"/page_1\">Goto page 1</a>"	
	);

	FixedEndpoint Page1(
		"<h1>Page 1</h1>"
		"<a href=\"/\">Back</a>"	
	);

	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/", &helloWorld);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/page_1", &Page1);

	std::vector<Client*> clients;
	fisk::tools::TCPListenSocket listenSocket(80);

	fisk::tools::EventReg newConnectionHandle = listenSocket.OnNewConnection.Register([&clients, &server](std::shared_ptr<fisk::tools::TCPSocket> aSocket)
	{
		clients.push_back(new Client(aSocket, server));
	});

	while (listenSocket.Update())
	{
		for (int i = static_cast<int>(clients.size()) - 1; i >= 0; i--)
			if (!clients[i]->Update())
				clients.erase(clients.begin() + i);
	}
}