
#include "tools/http/WebsocketEndpoint.h"

#include "tools/Json.h"
#include "tools/Base64.h"
#include "tools/Hexadecimal.h"

#include <sha1.hpp>

namespace fisk::tools::http
{
	WebsocketEndpoint::WebsocketEndpoint()
	{
		Json root;

		Json& body = root.AddChild("body");

		body.AddValue("code", 403);
		body.AddValue("message", "Forbidden");

		myErrorFrame.myCode = CommonResponseCodes::Forbidden;
		myErrorFrame.WriteJson(root);
		myErrorFrame.CalculateSize();
	}

	IConnection::RequestResult WebsocketEndpoint::OnFrame(const RequestFrame& aFrame, IConnection& aConnection)
	{
		IWebsocketCapable* conn = dynamic_cast<IWebsocketCapable*>(&aConnection);

		if (!conn)
			return aConnection.Send(myErrorFrame);

		if (!aFrame.ValidateHeader("Sec-WebSocket-Version", "13"))
			return aConnection.Send(myErrorFrame);
		if (!aFrame.ValidateHeader("Connection", "Upgrade"))
			return aConnection.Send(myErrorFrame);
		if (!aFrame.ValidateHeader("Upgrade", "websocket"))
			return aConnection.Send(myErrorFrame);
		if (!aFrame.HasHeader("Host"))
			return aConnection.Send(myErrorFrame);

		if (aFrame.HasHeader("Sec-WebSocket-Protocol"))
			return aConnection.Send(myErrorFrame);

		std::string key;
		if (!aFrame.GetHeader("Sec-WebSocket-Key", key))
			return aConnection.Send(myErrorFrame);

		ResponseFrame response;

		SHA1 sha1;

		sha1.update(key);
		sha1.update("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

		std::optional<std::vector<uint8_t>> sha1Data = HexaDecimal::TryDecode(sha1.final());

		if (!sha1Data)
			return aConnection.Send(myErrorFrame);


		response.myCode = CommonResponseCodes::SwitchingProtocols;
		response.SetOrVerifyHeader("Sec-WebSocket-Accept", Base64::Encode(*sha1Data));
		response.SetOrVerifyHeader("Connection", "Upgrade");
		response.SetOrVerifyHeader("Upgrade", "websocket");

		aConnection.Send(response);

		conn->UpgradeToWebsocket(this);

		return IConnection::RequestResult::Should_Terminate;
	}
}

