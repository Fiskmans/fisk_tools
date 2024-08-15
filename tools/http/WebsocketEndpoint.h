#pragma once

#include "tools/http/EndPoint.h"

namespace fisk::tools::http
{
	class IWebsocketCapableConnection : IConnection
	{
	public:
		virtual ~IWebsocketCapableConnection() = default;

		virtual void UpgradeToWebsocket() = 0;
	};

	class WebsocketEndpoint : public Endpoint
	{
	public:
		WebsocketEndpoint();

		// Inherited via Endpoint
		ResponseFrame OnFrame(const RequestFrame& aFrame, IConnection& aConnection) override;

	private:
		ResponseFrame myErrorFrame;
	};
}