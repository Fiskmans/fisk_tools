#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace fisk::tools
{
	class HexaDecimal
	{
	public:
		static std::optional<std::vector<uint8_t>> TryDecode(const std::string& aString);
		static std::string Encode(const std::vector<uint8_t>& aData);
	};
}