
#include "tools/http/WebsocketEndpoint.h"

#include "tools/Json.h"
#include "tools/Base64.h"

#include <Sha1.hpp>

namespace fisk::tools::http
{
	WebsocketEndpoint::WebsocketEndpoint(std::string aHost)
		: myHost(aHost)
	{
		Json root;

		Json& body = root.AddChild("body");

		body.AddValue("code", 403);
		body.AddValue("message", "Forbidden");

		myErrorFrame.myCode = CommonResponseCodes::Forbidden;
		myErrorFrame.WriteJson(root);
		myErrorFrame.CalculateSize();
	}

	ResponseFrame WebsocketEndpoint::OnFrame(const RequestFrame& aFrame, IConnection& aConnection)
	{
		IWebsocketCapableConnection* conn = dynamic_cast<IWebsocketCapableConnection*>(&aConnection);

		if (!conn)
			return myErrorFrame;

		if (!aFrame.ValidateHeader("Sec-WebSocket-Version", "13"))
			return myErrorFrame;
		if (!aFrame.ValidateHeader("Connection", "Upgrade"))
			return myErrorFrame;
		if (!aFrame.ValidateHeader("Upgrade", "websocket"))
			return myErrorFrame;
		if (!aFrame.HasHeader("Host"))
			return myErrorFrame;

		if (aFrame.HasHeader("Sec-WebSocket-Protocol"))
			return myErrorFrame;
		if (aFrame.HasHeader("Sec-WebSocket-Extensions"))
			return myErrorFrame;


		std::string key;
		if (!aFrame.GetHeader("Sec-WebSocket-Key", key))
			return myErrorFrame;

		ResponseFrame response;

		SHA1 sha1;

		Base64::Decode(key);

		//response.SetOrVerifyHeader("Sec-WebSocket-Accept", );
		response.SetOrVerifyHeader("Connection", "Upgrade");
		response.SetOrVerifyHeader("Upgrade", "websocket");

		return response;
	}
}

