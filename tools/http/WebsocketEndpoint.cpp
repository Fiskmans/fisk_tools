
#include "tools/http/WebsocketEndpoint.h"

#include "tools/Json.h"

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

	ResponseFrame WebsocketEndpoint::OnFrame(const RequestFrame& aFrame, IConnection& aConnection)
	{
		std::string value;
		if (!aFrame.GetHeader("Sec-WebSocket-Version"))
			return myErrorFrame;



		return ResponseFrame();
	}
}

