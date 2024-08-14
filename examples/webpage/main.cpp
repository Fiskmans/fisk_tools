

#include "tools/http/Server.h"
#include "tools/TCPSocket.h"
#include "tools/TCPListenSocket.h"

#include <iostream>

class FixedHtmlEndpoint : public fisk::tools::http::Endpoint
{
public:
	inline FixedHtmlEndpoint(std::string aContent)
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

class FixedJsonEndpoint : public fisk::tools::http::Endpoint
{
public:
	inline FixedJsonEndpoint(fisk::tools::Json& aContent)
	{
		myResponse.myCode = fisk::tools::http::CommonResponseCodes::OK;

		myResponse.WriteJson(aContent);
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

	FixedHtmlEndpoint helloWorld(
		"<h1>Hello world!</h1>"
		"<p><a href=\"/page_1\">Goto page 1</a></p>"
		"<p><a href=\"/page_2\">Goto page 2</a></p>"
		"<p><a href=\"/data\">Goto data page</a></p>"
	);

	FixedHtmlEndpoint Page1(
		"<h1>Page 1</h1>"
		"<p><a href=\"/\">Back</a></p>"
	);

	FixedHtmlEndpoint Page2(
		"<h1>Page 2</h1>"
		"<p><a href=\"/\">Back</a></p>"
	);

	fisk::tools::Json root;

	root.AddChild("body").AddValue("Message", "Hello world");

	FixedJsonEndpoint dataPage(root);

	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/", &helloWorld);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/page_1", &Page1);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/page_2", &Page2);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/data", &dataPage);

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