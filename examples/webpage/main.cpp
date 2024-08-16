

#include "tools/http/Server.h"
#include "tools/net/TCPSocket.h"
#include "tools/net/TCPListenSocket.h"
#include "tools/http/WebsocketEndpoint.h"
#include "tools/Trace.h"
#include "tools/Websocket.h"

#include <iostream>

class Client : public fisk::tools::http::IConnection, public fisk::tools::http::IWebsocketCapable
{
public:
	inline Client(std::shared_ptr<fisk::tools::TCPSocket> aSocket, fisk::tools::http::Server& aServer)
		: myWebsocket(aSocket->GetReadStream(), aSocket->GetWriteStream())
	{
		myWebsocketEnabled = false;
		mySocket = aSocket;
		myDataHandle = mySocket->OnDataAvailable.Register([this, &aServer]()
															{
																if (!myWebsocketEnabled)
																	aServer.UpdateOn(*this);

																if (myWebsocketEnabled)
																	myWebsocket.Update();
															});

		myRemoteAddress = mySocket->GetSocket().GetAddress();
	}

	bool Update()
	{
		return mySocket->Update();
	}

	void UpgradeToWebsocket(fisk::tools::http::WebsocketEndpoint* aUpgrader) override;

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
	bool myWebsocketEnabled;

	fisk::tools::Websocket myWebsocket;
	std::shared_ptr<fisk::tools::TCPSocket> mySocket;
	fisk::tools::EventReg myDataHandle;
	fisk::tools::EventReg myMessageEventHandle;
	std::string myRemoteAddress;

};

class StaticHtmlEndpoint : public fisk::tools::http::Endpoint
{
public:
	inline StaticHtmlEndpoint(std::string aContent)
	{
		myResponse.myCode = fisk::tools::http::CommonResponseCodes::Ok;

		myResponse.WriteRawHtml(aContent);
		myResponse.CalculateSize();
	}

	fisk::tools::http::IConnection::RequestResult OnFrame(const fisk::tools::http::RequestFrame& aFrame, fisk::tools::http::IConnection& aConnection) override
	{
		FISK_TRACE("http_response");

		if (Client* client = dynamic_cast<Client*>(&aConnection))
			std::cout << "served static html to " << client->Address() << "\n";
		else
			std::cout << "Unkown client type\n";

		return aConnection.Send(myResponse);
	}

private:
	fisk::tools::http::ResponseFrame myResponse;
};

class FixedJsonEndpoint : public fisk::tools::http::Endpoint
{
public:
	inline FixedJsonEndpoint(fisk::tools::Json& aContent)
	{
		myResponse.myCode = fisk::tools::http::CommonResponseCodes::Ok;

		myResponse.WriteJson(aContent);
		myResponse.CalculateSize();
	}

	fisk::tools::http::IConnection::RequestResult OnFrame(const fisk::tools::http::RequestFrame& aFrame, fisk::tools::http::IConnection& aConnection) override
	{
		FISK_TRACE("json_response");

		if (Client* client = dynamic_cast<Client*>(&aConnection))
			std::cout << "served static json to " << client->Address() << "\n";
		else
			std::cout << "Unkown client type\n";

		return aConnection.Send(myResponse);
	}

private:
	fisk::tools::http::ResponseFrame myResponse;
};

class TraceEndpoint : public fisk::tools::http::Endpoint
{
public:
	fisk::tools::http::IConnection::RequestResult OnFrame(const fisk::tools::http::RequestFrame& aFrame, fisk::tools::http::IConnection& aConnection) override
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

		return aConnection.Send(response);
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


struct Message
{
	std::string mySender;
	std::string myMessage;
};

class MessageDatabase
{
public:
	void Insert(Message aMessage);

	fisk::tools::Event<Message> OnMessage;

	std::vector<Message> GetAll();

private:
	std::vector<Message> myMessages;
};

class ChatEndpoint : public fisk::tools::http::Endpoint
{
public:
	ChatEndpoint(MessageDatabase& aDatabase)
		: myDatabase(aDatabase)
	{
		fisk::tools::Json success;

		{
			fisk::tools::Json& body = success.AddChild("body");
			
			body.AddValue("code", 200);
			body.AddValue("message", "ok");
		}

		mySuccess.myCode = fisk::tools::http::CommonResponseCodes::Ok;
		mySuccess.WriteJson(success);
		mySuccess.CalculateSize();

		fisk::tools::Json fail;

		{
			fisk::tools::Json& body = fail.AddChild("body");

			body.AddValue("code", 403);
			body.AddValue("message", "Forbidden");
		}

		myFailure.myCode = fisk::tools::http::CommonResponseCodes::Forbidden;
		myFailure.WriteJson(fail);
		myFailure.CalculateSize();
	}

	fisk::tools::http::IConnection::RequestResult OnFrame(const fisk::tools::http::RequestFrame& aFrame, fisk::tools::http::IConnection& aConnection) override
	{
		fisk::tools::Json root;
		if (!aFrame.GetAsJson(root))
			return aConnection.Send(mySuccess);

		Message message;

		if (!root["body"]["name"].GetIf(message.mySender))
			return aConnection.Send(mySuccess);

		if (!root["body"]["message"].GetIf(message.myMessage))
			return aConnection.Send(mySuccess);

		myDatabase.Insert(message);

		return aConnection.Send(mySuccess);
	}

private:

	MessageDatabase& myDatabase;
	fisk::tools::http::ResponseFrame mySuccess;
	fisk::tools::http::ResponseFrame myFailure;
};

class ChatSocketEndpoint : public fisk::tools::http::WebsocketEndpoint
{
public:
	ChatSocketEndpoint(MessageDatabase& aDataBase)
		: myDataBase(aDataBase)
	{
		myMessageHandle = aDataBase.OnMessage.Register([this](Message aMessage)
													   {
														   fisk::tools::Json root;

														   root.AddValue("type", "new_message");

														   fisk::tools::Json& body = root.AddChild("body");

														   body.AddValue("sender", aMessage.mySender);
														   body.AddValue("message", aMessage.myMessage);

														   OnData.Fire(&root);
													   });
	}


	void GetHistory(fisk::tools::Json& aOutRoot)
	{
		aOutRoot.AddValue("type", "history");

		fisk::tools::Json& body = aOutRoot.AddChild("body");

		fisk::tools::Json& messages = body.AddChild("messages");

		for (Message& message : myDataBase.GetAll())
		{
			fisk::tools::Json& messageJson = messages.PushChild();

			messageJson.AddValue("sender", message.mySender);
			messageJson.AddValue("message", message.myMessage);
		}
	}

	fisk::tools::Event<fisk::tools::Json*> OnData;

private:
	MessageDatabase& myDataBase;
	fisk::tools::EventReg myMessageHandle;
};

int main()
{
	fisk::tools::http::Server server;

	StaticHtmlEndpoint helloWorld(
		"<h1>Hello world!</h1>"
		"<p><a href=\"/page_1\">Goto page 1</a></p>"
		"<p><a href=\"/page_2\">Goto page 2</a></p>"
		"<p><a href=\"/chat\">Goto Chat page</a></p>"
		"<p><a href=\"/data\">Goto data page</a></p>"
		"<p><a href=\"/trace\">Goto trace page</a></p>"
	);

	StaticHtmlEndpoint Page1(
		"<h1>Page 1</h1>"
		"<p><a href=\"/\">Back</a></p>"
	);

	StaticHtmlEndpoint Page2(
		"<h1>Page 2</h1>"
		"<p><a href=\"/\">Back</a></p>"
	);

	StaticHtmlEndpoint ChatPage(
		R"(<h1>Chat</h1>)"
		R"(<div id="chatting_box">)"
			R"-(<a>Name:</a>)-"
			R"-(<input type="text" id="name_box" value="Guest">)-"
			
			R"(<br>)"
			R"-(<a>Message:</a>)-"
			R"-(<input type="text" id="message_box">)-"

			R"(<br>)"
			R"-(<button type="button" onclick="SendMessage()">Send</button>)-"
		R"(</div>)"
		R"(<div id="message_box"/>)"

		R"(<script>)"
		R"(
		// ES6 class 
		class EasyHTTP { 
  
		   // Make an HTTP PUT Request 
		   async put(url, data) { 
  
			// Awaiting fetch which contains method, 
			// headers and content-type and body 
			const response = await fetch(url, { 
			  method: 'PUT', 
			  headers: { 
				'Content-type': 'application/json'
			  }, 
			  body: JSON.stringify(data) 
			}); 
      
			// Awaiting response.json() 
			const resData = await response.json(); 
  
			// Return response data  
			return resData; 
		  } 
		}

		function SendMessage()
		{
			let messageBox = document.getElementById("message_box");
			let nameBox = document.getElementById("name_box");

			let payload = {
				body: {
					name: nameBox.value,
					message: messageBox.value
				}
			};

			messageBox.value = "";
			
			const http = new EasyHTTP; 
			http.put("/send_chat", payload)
				.then(data => console.log("Got", data)) 
				.catch(err => console.log(err));

			console.log("sent", payload);
		}

		let ws = new WebSocket("chat_socket");

		ws.addEventListener("open", (event) => {
			console.log("Connected");
		});

		ws.addEventListener("message", (event) =>
		{
			console.log("Message from server ", event.data);
		});

		)"
		R"(</script>)"
	);

	fisk::tools::Json root;

	root.AddChild("body").AddValue("Message", "Hello world");

	MessageDatabase database;

	FixedJsonEndpoint dataPage(root);
	TraceEndpoint tracePage;
	ChatEndpoint sendChat(database);
	ChatSocketEndpoint chatSocket(database);

	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/", &helloWorld);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/page_1", &Page1);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/page_2", &Page2);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/chat", &ChatPage);
	server.AddEndpoint(fisk::tools::http::RequestFrame::PUT, fisk::tools::http::Server::FilterMode::Full, "/send_chat", &sendChat);
	server.AddEndpoint(fisk::tools::http::RequestFrame::GET, fisk::tools::http::Server::FilterMode::Full, "/chat_socket", &chatSocket);
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

void MessageDatabase::Insert(Message aMessage)
{
	myMessages.push_back(aMessage);
	OnMessage.Fire(aMessage);
}

std::vector<Message> MessageDatabase::GetAll()
{
	return myMessages;
}

void Client::UpgradeToWebsocket(fisk::tools::http::WebsocketEndpoint* aUpgrader)
{
	ChatSocketEndpoint* endpoint = dynamic_cast<ChatSocketEndpoint*>(aUpgrader);

	if (!endpoint)
	{
		mySocket->Close();
		return;
	}

	fisk::tools::Json history;
	endpoint->GetHistory(history);

	myWebsocket.SendText(history.Serialize());

	myMessageEventHandle = endpoint->OnData.Register([this](fisk::tools::Json* aData)
													 {
														 myWebsocket.SendText(aData->Serialize());
													 });

	myWebsocketEnabled = true;

	
}
