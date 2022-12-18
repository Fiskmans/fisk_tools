#include "tools/StreamReader.h"

namespace fisk::tools
{

	bool StreamReader::ReadRaw(uint8_t* aData, uint32_t aSize)
	{
		return myReadStream.Read(aData, aSize);
	}

	StreamReader::StreamReader(ReadStream& aReadStream)
		: myReadStream(aReadStream)
	{
	}

	bool StreamReader::Process(uint8_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int8_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint16_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int16_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint32_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int32_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint64_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int64_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
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