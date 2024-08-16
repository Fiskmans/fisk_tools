#pragma once

#include "tools/http/Frame.h"
#include "tools/http/IConnection.h"

namespace fisk::tools::http
{
	class Endpoint
	{
	public:
		virtual ~Endpoint() = default;


		virtual IConnection::RequestResult OnFrame(const RequestFrame& aFrame, IConnection& aConnection) = 0;
	};
}