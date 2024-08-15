

#include "tools/http/Server.h"
#include "tools/net/TCPSocket.h"
#include "tools/net/TCPListenSocket.h"
#include "tools/Trace.h"

#include <iostream>

class Client : public fisk::tools::http::IConnection
{
public:
	inline Client(std::shared_ptr<fisk::tools::TCPSocket> aSocket, fisk::tools::http::Server& aServer)
	{
		mySocket = aSocket;
		myDataHandle = mySocket->OnDataAvailable.Register([this, &aServer]()
														  {
															  aServer.UpdateOn(*this);
														  });

		myRemoteAddress = mySocket->GetSocket().GetAddress();
	}

	bool Update()
	{
		return mySocket->Update();
	}

	void UpgradeToWebsocket()
	{
	}

	std::string Address() const
	{
		return myRemoteAddress;
	}

	// Inherited via IConnection
	fisk::tools::ReadStream& GetReadStream() override
	{
		return mySocket->GetReadStream();
	}

	fisk::tools::WriteStream& GetWriteStream() override
	{
		return mySocket->GetWriteStream();
	}

private:
	std::shared_ptr<fisk::tools::TCPSocket> mySocket;
	fisk::tools::EventReg myDataHandle;
	std::string myRemoteAddress;

};

class FixedHtmlEndpoint : public fisk::tools::http::Endpoint
{
public:
	inline FixedHtmlEndpoint(std::string aContent)
	{
		myResponse.myCode = fisk::tools::http::CommonResponseCodes::OK;

		myResponse.WriteRawHtml(aContent);
		myResponse.CalculateSize();
	}

	fisk::tools::http::ResponseFrame OnFrame(const fisk::tools::http::RequestFrame& aFrame, fisk::tools::http::IConnection& aConnection) override
	{
		FISK_TRACE("http_response");

		if (Client* client = dynamic_cast<Client*>(&aConnection))
			std::cout << "served static html to " << client->Address() << "\n";
		else
			std::cout << "Unkown client type\n";

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

	fisk::tools::http::ResponseFrame OnFrame(const fisk::tools::http::RequestFrame& aFrame, fisk::tools::http::IConnection& aConnection) override
	{
		FISK_TRACE("json_response");

		if (Client* client = dynamic_cast<Client*>(&aConnection))
			std::cout << "served static json to " << client->Address() << "\n";
		else
			std::cout << "Unkown client type\n";

		return myResponse;
	}

private:
	fisk::tools::http::ResponseFrame myResponse;
};

class TraceEndpoint : public fisk::tools::http::Endpoint
{
public:
	fisk::tools::http::ResponseFrame OnFrame(const fisk::tools::http::RequestFrame& aFrame, fisk::tools::http::IConnection& aConnection) override
	{
		FISK_TRACE("trace_response");

		if (Client* client = dynamic_cast<Client*>(&aConnection))
			std::cout << "served trace dump to " << client->Address() << "\n";
		else
			std::cout << "Unkown client type\n";

		fisk::tools::http::ResponseFrame response;

		std::stringstream ss;

		ss << "<!DOCTYPE html>";

		ss << "<html>";

		{
			ss << "<head>";

			ss << "<title>Trace</title>";
			ss << R"(<meta http-equiv="refresh" content="1">)";

			ss << "</head>";
		}

		{
			ss << "<body>";

			ss << R"(<p><a href="/">Back</a></p>)";

			for (const fisk::tools::Trace& trace : fisk::tools::PerformanceTracer::GetRoots())
			{
				Build(ss, trace);
			}

			ss << "</body>";
		}
		ss << "</html>";

		response.WriteRawHtml(ss.str());
		response.CalculateSize();

		return response;
	}

private:
	void Build(std::ostream& aStream, const fisk::tools::Trace& aTrace)
	{
		{
			aStream << "<a>[" << aTrace.myTag << "]: </a>";

			aStream << "<a>";
			PrettyTime(aStream, aTrace.myTimeSpent);
			aStream << "</a>";
		}

		for (const fisk::tools::Trace& trace : aTrace.myChildren)
		{
			aStream << R"(<div style="padding-left:20px;">)";
			Build(aStream, trace);
			aStream << "</div>";
		}
	}

	void PrettyTime(std::ostream& aStream, std::chrono::nanoseconds aTime)
	{
	
		using namespace std::chrono_literals;

		if (aTime > 1s)
		{
			aStream << std::chrono::duration_cast<std::chrono::seconds>(aTime);
		}
		else if (aTime > 1ms)
		{
			aStream << std::chrono::duration_cast<std::chrono::milliseconds>(aTime);
		}
		else if (aTime > 1us)
		{
			aStream << std::chrono::duration_cast<std::chrono::microseconds>(aTime);
		}
		else
		{
			aStream << aTime;
		}
	}

private:
};


int main()
{
	fisk::tools::http::Server server;

	FixedHtmlEndpoint helloWorld(
		"<h1>Hello world!</h1>"
		"<p><a href=\"/page_1\">Goto page 1</a></p>"
		"<p><a href=\"/page_2\">Goto page 2</a></p>"
		"<p><a href=\"/data\">Goto data page</a></p>"
		"<p><a href=\"/trace\">Goto trace page</a></p>"
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
	TraceEndpoint tracePage;

	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/", &helloWorld);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/page_1", &Page1);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/page_2", &Page2);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/data", &dataPage);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/trace", &tracePage);

	std::vector<Client*> clients;
	fisk::tools::TCPListenSocket listenSocket(80);

	fisk::tools::EventReg newConnectionHandle = listenSocket.OnNewConnection.Register([&clients, &server](std::shared_ptr<fisk::tools::TCPSocket> aSocket)
	{
		clients.push_back(new Client(aSocket, server));
	});

	while (true)
	{
		FISK_TRACE("main");

		{
			FISK_TRACE("listen");
			if (!listenSocket.Update())
				break;
		}
		{
			FISK_TRACE("clients");
			for (int i = static_cast<int>(clients.size()) - 1; i >= 0; i--)
			{
				if (!clients[i]->Update())
				{
					FISK_TRACE("deleting");
					clients.erase(clients.begin() + i);
				}
			}
		}
	}
}