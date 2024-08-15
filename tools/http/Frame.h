#pragma once

#include "tools/Stream.h"
#include "tools/Json.h"

#include <string>
#include <optional>
#include <stdint.h>
#include <unordered_map>

namespace fisk::tools::http
{
	constexpr static char LineBreak[] = "\r\n";
	constexpr static char ProtocolVersion[] = "HTTP/1.1";

	struct RequestFrame
	{
	public:

		enum Method : uint8_t
		{
			POST    = 1 << 0,
			GET     = 1 << 1,
			PUT	    = 1 << 2,
			HEAD    = 1 << 3,
			DELETE  = 1 << 4,
			CONNECT = 1 << 5,
			OPTIONS = 1 << 6,
			TRACE_  = 1 << 7 // name collision with macro
		};

		static std::optional<RequestFrame> FromStream(ReadStream& aStream);

		bool SetMethod(std::string aMethod);
		bool HasHeader(std::string aField) const;
		bool GetHeader(std::string aField, std::string& aOutValue) const;

		Method myMethod;
		std::string myPath;
		std::unordered_map<std::string, std::string> myHeaders;
		std::vector<unsigned char> myData;

	private:

		static bool TryReadLine(ReadStream& aStream, std::string& aOutLine);
	};

	struct ResponseCode
	{
		std::string ToString();

		int myCode;
		std::string myMessage;
	};

	namespace CommonResponseCodes
	{
		inline ResponseCode OK{ 200, "OK" };
		inline ResponseCode Forbidden{ 403, "Forbidden" };
		inline ResponseCode NOT_Found{ 404, "Not Found" };
	}

	struct ResponseFrame
	{
	public:
		constexpr static char LineBreak[] = "\r\n";
		constexpr static char ProtocolVersion[] = "HTTP/1.1";

		enum Method : uint8_t
		{
			OK = 1 << 0
		};

		void ToStream(WriteStream& aStream);

		void WriteText(std::string aText);
		void WriteTextLine(std::string aText);
		void WriteJson(const Json& aJson);

		void WriteRawHtml(std::string aHtml);

		void SetOrVerifyHeader(std::string aName, std::string aValue);

		void CalculateSize();

		Method myMethod;
		ResponseCode myCode;
		std::unordered_map<std::string, std::string> myHeaders;
		std::vector<unsigned char> myData;

	private:
		void StringToStream(WriteStream& aStream, std::string aString);
	};
}