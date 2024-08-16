
#include "tools/Hexadecimal.h"

#if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable > 202202L
#include <utility>
#endif

namespace fisk::tools 
{
	namespace hexadecimal
	{
		uint8_t NibbleFromChar(char aChar)
		{
			switch (aChar)
			{
			case '0':
				return 0;
			case '1':
				return 1;
			case '2':
				return 2;
			case '3':
				return 3;
			case '4':
				return 4;
			case '5':
				return 5;
			case '6':
				return 6;
			case '7':
				return 7;
			case '8':
				return 8;
			case '9':
				return 9;
			case 'a':
			case 'A':
				return 10;
			case 'b':
			case 'B':
				return 11;
			case 'c':
			case 'C':
				return 12;
			case 'd':
			case 'D':
				return 13;
			case 'e':
			case 'E':
				return 14;
			case 'f':
			case 'F':
				return 15;
			}
#if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable > 202202L
			std::unreachable();
#endif
			return 0;
		}

		char CharFromNibble(uint8_t aNibble)
		{
			switch (aNibble)
			{
			case 0:
				return '0';
			case 1:
				return '1';
			case 2:
				return '2';
			case 3:
				return '3';
			case 4:
				return '4';
			case 5:
				return '5';
			case 6:
				return '6';
			case 7:
				return '7';
			case 8:
				return '8';
			case 9:
				return '9';
			case 10:
				return 'A';
			case 11:
				return 'B';
			case 12:
				return 'C';
			case 13:
				return 'D';
			case 14:
				return 'E';
			case 15:
				return 'F';
			}

#if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable > 202202L
			std::unreachable();
#endif
			return '?';
		}
	}

	std::optional<std::vector<uint8_t>> HexaDecimal::TryDecode(const std::string& aString)
	{
		if (aString.length() % 2 != 0)
			return {};

		if (aString.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos)
			return {};

		std::vector<uint8_t> out;

		size_t length = aString.length() / 2;

		out.reserve(length);

		for (size_t i = 0; i < length; i++)
		{
			out.push_back(
				(hexadecimal::NibbleFromChar(aString[i * 2 + 0]) << 4)
				| hexadecimal::NibbleFromChar(aString[i * 2 + 1]));
		}

		return out;
	}

	std::string HexaDecimal::Encode(const std::vector<uint8_t>& aData)
	{
		std::string out;

		out.reserve(aData.size() * 2);

		for (uint8_t byte : aData)
		{
			out += hexadecimal::CharFromNibble((byte & 0xF0) >> 4);
			out += hexadecimal::CharFromNibble(byte & 0x0F);
		}

		return out;
	}
}
