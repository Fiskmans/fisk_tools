#pragma once

#include "tools/http/EndPoint.h"

namespace fisk::tools::http
{
	class WebsocketEndpoint;

	class IWebsocketCapable
	{
	public:
		virtual ~IWebsocketCapable() = default;

		virtual void UpgradeToWebsocket(WebsocketEndpoint* aUpgrader) = 0;
	};

	class WebsocketEndpoint : public Endpoint
	{
	public:
		WebsocketEndpoint();

		// Inherited via Endpoint
		IConnection::RequestResult OnFrame(const RequestFrame& aFrame, IConnection& aConnection) override;

	private:
		ResponseFrame myErrorFrame;
	};
}