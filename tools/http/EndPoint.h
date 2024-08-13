#pragma once

#include "tools/http/Frame.h"

namespace fisk::tools::http
{
	class Endpoint
	{
	public:
		virtual ~Endpoint() = default;

		virtual ResponseFrame OnFrame(const RequestFrame& aFrame) = 0;
	};
}