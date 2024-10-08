#include "tools/http/Frame.h"

#include <sstream>
#include <cassert>
#include <algorithm>
#include <charconv>
#include <ranges>

namespace fisk::tools::http
{
	struct Trim
	{
		std::string operator()(std::string aString)
		{
			static const char* whitespace = " \t\n\v";

			std::string::size_type start = aString.find_first_not_of(whitespace);
			std::string::size_type end = aString.find_last_not_of(whitespace);

			if (start == std::string::npos)
				return "";

			assert(end != std::string::npos);

			return aString.substr(start, end - start + 1);
		}
	};

	std::optional<RequestFrame> RequestFrame::FromStream(ReadStream& aStream)
	{
		aStream.RestoreRead();

		RequestFrame frame;

		std::string protoLine;

		if (!TryReadLine(aStream, protoLine))
			return {};

		std::stringstream protoLineStream(protoLine);
		std::string method;
		std::string proto;

		if (!(protoLineStream >> method >> frame.myPath >> proto))
			return {}; // malformed proto line

		if (!frame.SetMethod(method))
			return {};

		if (proto != ProtocolVersion)
			return {};

		std::string headerLine;

		while (true)
		{
			if (!TryReadLine(aStream, headerLine))
				return {};

			if (headerLine.empty())
				break;

			std::string::size_type at = headerLine.find(": ");

			if (at == std::string::npos)
				return {}; // malformed header line

			std::string key = headerLine.substr(0, at);
			std::string value = headerLine.substr(at + 2);

			if (frame.myHeaders.find(key) != frame.myHeaders.end())
				return {}; // duplicate header value

			frame.myHeaders[key] = value;
		}

		auto contentLengthIt = frame.myHeaders.find("Content-Length");

		if (contentLengthIt != frame.myHeaders.end())
		{
			long length;

			std::from_chars_result contentLengthParseResult = std::from_chars(contentLengthIt->second.c_str(), contentLengthIt->second.c_str() + contentLengthIt->second.length(), length);

			if (contentLengthParseResult.ec != std::errc{})
				return {};

			frame.myData.resize(length);

			if (!aStream.Read(frame.myData.data(), length))
				return {};
		}

		aStream.CommitRead();

		return frame;
	}

	bool RequestFrame::SetMethod(std::string aMethod)
	{
		if (aMethod == "POST")
		{
			myMethod = Method::POST;
			return true;
		}
		if (aMethod == "GET")
		{
			myMethod = Method::GET;
			return true;
		}
		if (aMethod == "PUT")
		{
			myMethod = Method::PUT;
			return true;
		}
		if (aMethod == "HEAD")
		{
			myMethod = Method::HEAD;
			return true;
		}
		if (aMethod == "DELETE")
		{
			myMethod = Method::DELETE;
			return true;
		}
		if (aMethod == "CONNECT")
		{
			myMethod = Method::CONNECT;
			return true;
		}
		if (aMethod == "OPTIONS")
		{
			myMethod = Method::OPTIONS;
			return true;
		}
		if (aMethod == "TRACE")
		{
			myMethod = Method::TRACE_;
			return true;
		}

		return false;
	}

	bool RequestFrame::HasHeader(std::string aField) const
	{
		return myHeaders.find(aField) != myHeaders.end();
	}

	bool RequestFrame::GetHeader(std::string aField, std::string& aOutValue) const
	{
		auto it = myHeaders.find(aField);
		if (it != myHeaders.end())
		{
			aOutValue = it->second;
			return true;
		}

		return false;
	}

	bool RequestFrame::ValidateHeader(std::string aField, std::string aExpectedValue) const
	{
		auto it = myHeaders.find(aField);
		return it != myHeaders.end() && it->second == aExpectedValue;
	}

	bool RequestFrame::ValidateHeaderContains(std::string aField, std::string aExpectedValue) const
	{
		std::string full;

		if (!GetHeader(aField, full))
			return false;

		auto values = std::ranges::views::split(std::string_view(full), std::string_view(","))
				| std::ranges::views::transform(
					[](auto aCharRange) 
					{ 
						return std::string(std::ranges::begin(aCharRange), std::ranges::end(aCharRange)); 
					})
				| std::ranges::views::transform(Trim{});

		return std::ranges::any_of(values, 
			[&aExpectedValue](std::string aValue) 
			{ 
				return aValue == aExpectedValue; 
			});
	}

	bool RequestFrame::GetAsJson(fisk::tools::Json& aOutRoot) const
	{
		return aOutRoot.Parse(std::string(myData.begin(), myData.end()).c_str());
	}

	bool RequestFrame::TryReadLine(ReadStream& aStream, std::string& aOutLine)
	{
		constexpr size_t lineBreakLength = sizeof(LineBreak) - 1;

		auto availabe = aStream.AvailableData();

		auto from = std::begin(availabe);
		auto to = std::end(availabe);

		auto at = std::search(from, to, LineBreak, LineBreak + lineBreakLength);

		if (at == to)
			return false;

		auto length = std::distance(from, at);

		aOutLine.resize(length);
		aStream.Read(reinterpret_cast<uint8_t*>(aOutLine.data()), length);

		uint8_t _[lineBreakLength];
		aStream.Read(_, lineBreakLength);

		return true;
	}

	void ResponseFrame::ToStream(WriteStream& aStream)
	{
		StringToStream(aStream, ProtocolVersion);
		StringToStream(aStream, " ");
		StringToStream(aStream, myCode.ToString());
		StringToStream(aStream, LineBreak);

		for (std::pair<std::string, std::string> kvPair : myHeaders)
		{
			StringToStream(aStream, kvPair.first);
			StringToStream(aStream, ": ");
			StringToStream(aStream, kvPair.second);
			StringToStream(aStream, LineBreak);
		}

		StringToStream(aStream, LineBreak);

		aStream.WriteData(myData.data(), myData.size());
	}

	void ResponseFrame::WriteText(std::string aText)
	{
		SetOrVerifyHeader("Content-Type", "text/plain");

		myData.reserve(myData.size() + aText.length());

		std::copy(aText.begin(), aText.end(), std::back_inserter(myData));
	}

	void ResponseFrame::WriteTextLine(std::string aText)
	{
		SetOrVerifyHeader("Content-Type", "text/plain");

		myData.reserve(myData.size() + aText.length() + 2);

		std::copy(std::begin(aText), std::end(aText), std::back_inserter(myData));
		std::copy(std::begin(LineBreak), std::end(LineBreak), std::back_inserter(myData));
	}

	void ResponseFrame::WriteJson(const Json& aJson)
	{
		SetOrVerifyHeader("Content-Type", "text/json");

		std::string data = aJson.Serialize();

		myData.reserve(myData.size() + data.length());
		std::copy(std::begin(data), std::end(data), std::back_inserter(myData));
	}

	void ResponseFrame::WriteRawHtml(std::string aHtml)
	{
		SetOrVerifyHeader("Content-Type", "text/html");

		myData.reserve(myData.size() + aHtml.length());
		std::copy(std::begin(aHtml), std::end(aHtml), std::back_inserter(myData));
	}

	void ResponseFrame::SetOrVerifyHeader(std::string aName, std::string aValue)
	{
		decltype(myHeaders)::iterator elem = myHeaders.find(aName);
		if (elem == myHeaders.end())
		{
			myHeaders.insert({ aName, aValue });
			return;
		}
		
		assert(elem->second == aValue);
	}

	void ResponseFrame::CalculateSize()
	{
		SetOrVerifyHeader("Content-Length", std::to_string(myData.size()));
	}

	void ResponseFrame::StringToStream(WriteStream& aStream, std::string aString)
	{
		aStream.WriteData(reinterpret_cast<const uint8_t*>(aString.c_str()), aString.length());
	}

	std::string ResponseCode::ToString()
	{
		return std::to_string(myCode) + " " + myMessage;
	}
}
