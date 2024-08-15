#pragma once

#include <vector>
#include <string>
#include <string_view>

namespace fisk::tools
{
	class Base64
	{
	public:
		static std::string Encode(const std::vector<uint8_t>& aData);
		static std::vector<uint8_t> Decode(const std::string_view aData);
	};
}