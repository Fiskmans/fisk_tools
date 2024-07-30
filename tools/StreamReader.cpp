#include "tools/StreamReader.h"

#include <bit>

#include <cstring>

namespace fisk::tools
{
	bool StreamReader::ReadRawNumeric(uint8_t* aData, uint32_t aSize)
	{
		if constexpr (std::endian::native == std::endian::big) 
			return ReadRaw(aData, aSize);
		else if constexpr (std::endian::native == std::endian::little) 
			return ReadRawReversed(aData, aSize);
	}

	bool StreamReader::ReadRawReversed(uint8_t* aData, uint32_t aSize)
	{
		if (!myReadStream.Read(aData, aSize))
			return false;

		std::reverse(aData, aData + aSize);

		return true;
	}

	bool StreamReader::ReadRaw(uint8_t* aData, uint32_t aSize)
	{
		return myReadStream.Read(aData, aSize);
	}

	StreamReader::StreamReader(ReadStream& aReadStream)
		: myReadStream(aReadStream)
	{
	}

	bool StreamReader::Process(bool& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint8_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int8_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint16_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int16_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint32_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int32_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint64_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int64_t& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(float& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(double& aValue)
	{
		return ReadRawNumeric(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(std::string& aValue)
	{
		aValue = "";
		uint8_t chunk[64];

		while (true)
		{
			size_t amount = myReadStream.Peek(chunk, 64);

			size_t nullTerminator = 0;
			for (; nullTerminator < amount; nullTerminator++)
				if (chunk[nullTerminator] == '\0')
					break;

			if (nullTerminator != amount)
				assert(myReadStream.Read(chunk, nullTerminator + 1));
			else
				assert(myReadStream.Read(chunk, amount));

			aValue.append(reinterpret_cast<const char*>(chunk), nullTerminator);

			if (nullTerminator != amount)
				if (chunk[nullTerminator] == '\0')
					break;

			if (amount != 64)
				return false;
		}

		return true;
	}

} // namespace fisk::tools