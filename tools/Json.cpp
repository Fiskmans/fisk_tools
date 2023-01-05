#include "tools/JSON.h"

#include "tools/Iterators.h"

#include <cassert>
#include <charconv>
#include <iostream>
#include <sstream>
#include <stack>

namespace fisk::tools
{
	namespace json_help
	{
		bool IsWhiteSpace(char aChar)
		{
			return !isprint(aChar) || aChar == ' ';
		}

		void FindStart(const char*& aPtr)
		{
			while (*aPtr != '\0')
			{
				if (!IsWhiteSpace(*aPtr))
					break;

				aPtr++;
			}
		}

		void FindEndOfString(const char*& aPtr)
		{
			while (*aPtr != '\0')
			{
				if (*aPtr == '"')
					break;

				aPtr++;
				if (*aPtr == '\\')
					aPtr++;
			}
		}

		bool ContainsChar(const char* aBegin, const char* aEnd, char aValue)
		{
			for (const char* begin = aBegin; begin < aEnd; begin++)
			{
				if (*begin == aValue)
				{
					return true;
				}
			}
			return false;
		}

		bool EscapeString(char*& aPtr)
		{
			char* read = aPtr;
			while (*read != '\0')
			{
				if (*read == '\\')
				{
					read++;
					if (*read == '\0')
						return false;

					switch (*read)
					{
					case 'n':
						*aPtr = '\n';
						break;
					case 'r':
						*aPtr = '\r';
						break;
					case 't':
						*aPtr = '\t';
						break;
					case 'b':
						*aPtr = '\b';
						break;
					case 'f':
						*aPtr = '\f';
						break;
					case '\\':
						*aPtr = '\\';
						break;
					case '"':
						*aPtr = '\"';
						break;
					case '\'':
						*aPtr = '\'';
						break;
					default:
						return false;
					}
				}
				else
				{
					*aPtr = *read;
				}
				read++;
				aPtr++;
			}
			return true;
		}

		void WriteAndIndent(std::stringstream& aStream, const std::string& aString)
		{
			size_t at = 0;
			while (true)
			{
				size_t next = aString.find('\n');
				if (next == std::string::npos)
					break;

				aStream << aString.substr(at, next - at + 1);
				aStream << '\t';
				at = next + 1;
			}

			aStream << aString.substr(at);
		}
	} // namespace json_help

	JSONObject JSONObject::NullObject;

	bool JSONObject::Parse(const char* aString)
	{
		const char* ignored = aString;
		return ParseInternal(ignored);
	}

	JSONObject& JSONObject::operator[](const char* aKey) const
	{
		if (IsNull())
			return NullObject;

		if (!std::holds_alternative<ObjectType>(myValue))
			return NullObject;

		const ObjectType& children = std::get<ObjectType>(myValue);

		ObjectType::const_iterator it = children.find(aKey);

		if (it == children.end())
			return NullObject;

		return *(it->second);
	}

	JSONObject& JSONObject::operator[](int aIndex) const
	{
		if (IsNull())
			return NullObject;

		if (!std::holds_alternative<ArrayType>(myValue))
			return NullObject;

		const ArrayType& children = std::get<ArrayType>(myValue);

		if (aIndex >= children.size())
			return NullObject;

		return *children[aIndex];
	}

	bool JSONObject::HasChild(const char* aKey) const
	{
		return &(operator[](aKey)) != &(NullObject);
	}

	void JSONObject::AddChild(const std::string& aKey, std::unique_ptr<JSONObject> aChild)
	{
		assert(aChild);

		if (std::holds_alternative<NullType>(myValue))
			myValue = ObjectType();

		if (!std::holds_alternative<ObjectType>(myValue))
			return;

		ObjectType& children = std::get<ObjectType>(myValue);

		children[aKey] = std::move(aChild);
	}

	void JSONObject::PushChild(std::unique_ptr<JSONObject> aChild)
	{
		assert(aChild);

		if (std::holds_alternative<NullType>(myValue))
			myValue = ArrayType();

		if (!std::holds_alternative<ArrayType>(myValue))
			return;

		std::get<ArrayType>(myValue).emplace_back(std::move(aChild));
	}

	bool tools::JSONObject::IsNull() const
	{
		return std::holds_alternative<NullType>(myValue);
	}

	tools::JSONObject::operator bool() const
	{
		return !IsNull();
	}

	std::string JSONObject::Serialize(bool aPretty)
	{
		std::stringstream stream;
		switch (myValue.index())
		{
		case 0:
			stream << "null";
			break;
		case 1:
			stream << std::get<NumberType>(myValue);
			break;
		case 2:
			stream << '"' << std::get<StringType>(myValue) << '"';
			break;
		case 3:
			stream << std::get<BooleanType>(myValue) ? "true" : "false";
			break;
		case 4:
			{
				ArrayType& children = std::get<ArrayType>(myValue);

				bool containExpanded = false;
				for (std::unique_ptr<JSONObject>& child : children)
				{
					switch (child->myValue.index())
					{
					case 4:
					case 5:
						containExpanded = true;
						break;
					}
				}
				stream << '[';
				if (aPretty && containExpanded)
					stream << "\n\t";

				bool first = true;
				for (std::unique_ptr<JSONObject>& child : children)
				{
					if (!first)
					{
						if (aPretty && containExpanded)
							stream << ",\n\t";
						else
							stream << ',';
					}

					if (aPretty)
						json_help::WriteAndIndent(stream, child->Serialize(true));
					else
						stream << child->Serialize(false);

					first = false;
				}

				if (aPretty && containExpanded)
					stream << '\n';

				stream << ']';
			}
			break;
		case 5:
			{
				ObjectType& children = std::get<ObjectType>(myValue);
				stream << '[';

				for (auto& kvPair : children)
				{
					if (aPretty)
						stream << "\n\t";

					stream << '"';
					stream << kvPair.first;
					stream << "\":";

					if (aPretty)
						stream << ' ';

					if (aPretty)
						json_help::WriteAndIndent(stream, kvPair.second->Serialize(true));
					else
						stream << kvPair.second->Serialize(false);
				}

				if (aPretty)
					stream << '\n';

				stream << '}';
			}
			break;
		}

		return stream.str();
	}

	JSONObject& tools::JSONObject::operator=(const NumberType& aValue)
	{
		myValue = aValue;
		return *this;
	}

	JSONObject& tools::JSONObject::operator=(const StringType& aValue)
	{
		myValue = aValue;
		return *this;
	}

	JSONObject& tools::JSONObject::operator=(const BooleanType& aValue)
	{
		myValue = aValue;
		return *this;
	}

	bool tools::JSONObject::GetIf(long long& aValue) const
	{
		if (!std::holds_alternative<NumberType>(myValue))
			return false;

		aValue = static_cast<long long>(std::get<NumberType>(myValue));
		return true;
	}

	bool tools::JSONObject::GetIf(long& aValue) const
	{
		if (!std::holds_alternative<NumberType>(myValue))
			return false;

		aValue = static_cast<long>(std::get<NumberType>(myValue));
		return true;
	}

	bool tools::JSONObject::GetIf(size_t& aValue) const
	{
		if (!std::holds_alternative<NumberType>(myValue))
			return false;

		aValue = static_cast<size_t>(std::get<NumberType>(myValue));
		return true;
	}

	bool tools::JSONObject::GetIf(int& aValue) const
	{
		if (!std::holds_alternative<NumberType>(myValue))
			return false;

		aValue = static_cast<int>(std::get<NumberType>(myValue));
		return true;
	}

	bool tools::JSONObject::GetIf(double& aValue) const
	{
		if (!std::holds_alternative<NumberType>(myValue))
			return false;

		aValue = static_cast<double>(std::get<NumberType>(myValue));
		return true;
	}

	bool tools::JSONObject::GetIf(float& aValue) const
	{
		if (!std::holds_alternative<NumberType>(myValue))
			return false;

		aValue = static_cast<float>(std::get<NumberType>(myValue));
		return true;
	}

	bool tools::JSONObject::GetIf(std::string& aValue) const
	{
		if (!std::holds_alternative<StringType>(myValue))
			return false;

		aValue = std::get<StringType>(myValue);
		return true;
	}

	bool tools::JSONObject::GetIf(bool& aValue) const
	{
		if (!std::holds_alternative<BooleanType>(myValue))
			return false;

		aValue = std::get<BooleanType>(myValue);
		return true;
	}

	bool tools::JSONObject::GetIf(ArrayType*& aValue) 
	{
		if (!std::holds_alternative<ArrayType>(myValue))
			return false;

		aValue = &std::get<ArrayType>(myValue);
		return true;
	}

	bool tools::JSONObject::GetIf(ObjectType*& aValue)
	{
		if (!std::holds_alternative<ObjectType>(myValue))
			return false;

		aValue = &std::get<ObjectType>(myValue);
		return true;
	}

	bool tools::JSONObject::ParseInternal(const char*& aString)
	{
		const char*& at = aString;

		json_help::FindStart(at);

		if (*at == '\0')
			return false;

		switch (*at)
		{
		case '[':
			myValue = ArrayType();
			break;
		case '{':
			myValue = ObjectType();
			break;
		default:
			return ParseAsValue(at);
		}

		at++;

		json_help::FindStart(at);

		if (*at == '\0')
			return false;

		if (std::holds_alternative<ObjectType>(myValue))
		{
			if (*at == '}')
				return true;
		}
		else if (std::holds_alternative<ArrayType>(myValue))
		{
			if (*at == ']')
				return true;
		}


		while (*at != '\0')
		{
			json_help::FindStart(at);

			if (std::holds_alternative<ObjectType>(myValue))
			{
				if (*at != '"')
					return false;

				at++;

				if (*at == '\0')
					return false;

				const char* startOfName = at;

				json_help::FindEndOfString(at);

				if (*at != '"')
					return false;

				const char* nameEnd = at;

				at++;

				json_help::FindStart(at);

				if (*at != ':')
					return false;
				
				at++;
				json_help::FindStart(at);

				if (*at == '\0')
					return false;

				std::unique_ptr<JSONObject> child = std::make_unique<JSONObject>();
				if (!child->ParseInternal(at))
					return false;

				AddChild(std::string(startOfName, nameEnd), std::move(child));
			}
			else if (std::holds_alternative<ArrayType>(myValue))
			{
				std::unique_ptr<JSONObject> child = std::make_unique<JSONObject>();
				if (!child->ParseInternal(at))
					return false;

				PushChild(std::move(child));
			}
			else
			{
				assert(false);
			}

			json_help::FindStart(at);

			if (std::holds_alternative<ObjectType>(myValue))
			{
				if (*at == '}')
					break;
			}
			else if (std::holds_alternative<ArrayType>(myValue))
			{
				if (*at == ']')
					break;
			}
			else
			{
				assert(false);
			}

			if (*at == ',')
				return false;

			at++;
		}

		if (*at == '\0')
			return false;

		return true;
	}

	bool JSONObject::ParseAsValue(const char*& aPtr)
	{
		switch (*aPtr)
		{
		case '"':
			{
				aPtr++;
				const char* startOfString = aPtr;
				json_help::FindEndOfString(aPtr);
				if (*aPtr != '"')
					return false;

				size_t length = std::distance(startOfString, aPtr);
				char* buffer  = static_cast<char*>(alloca(length + 1));
				memcpy(buffer, startOfString, length);
				buffer[length] = '\0';

				char* endOfString = buffer;
				if (!json_help::EscapeString(endOfString))
					return false;

				myValue = StringType(buffer, endOfString);
				aPtr++;
			}
			return true;

		case 't':
		case 'T':
			aPtr++;
			if (*aPtr != 'r' && *aPtr != 'R')
				return false;

			aPtr++;
			if (*aPtr != 'u' && *aPtr != 'U')
				return false;

			aPtr++;
			if (*aPtr != 'e' && *aPtr != 'E')
				return false;

			aPtr++;
			myValue = true;
			return true;

		case 'n':
		case 'N':
			aPtr++;
			if (*aPtr != 'u' && *aPtr != 'U')
				return false;

			aPtr++;
			if (*aPtr != 'l' && *aPtr != 'L')
				return false;

			aPtr++;
			if (*aPtr != 'l' && *aPtr != 'L')
				return false;

			aPtr++;
			myValue = nullptr_t();
			return true;

		case 'f':
		case 'F':
			aPtr++;
			if (*aPtr != 'a' && *aPtr != 'A')
				return false;

			aPtr++;
			if (*aPtr != 'l' && *aPtr != 'L')
				return false;

			aPtr++;
			if (*aPtr != 's' && *aPtr != 'S')
				return false;

			aPtr++;
			if (*aPtr != 'e' && *aPtr != 'E')
				return false;

			aPtr++;
			myValue = false;
			return true;
		default:
			break;
		}

		const char* aStart = aPtr;
		while (*aPtr != '\0')
		{
			switch (*aPtr)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '.':
			case 'e':
				aPtr++;
				continue;
			default:
				break;
			}
			break;
		}

		double val;
		auto result = std::from_chars(aStart, aPtr, val);
		if (result.ptr != aPtr)
			return false;
		if (result.ec == std::errc::result_out_of_range)
			return false;

		myValue = val;

		return true;
	}

} // namespace fisk::tools
