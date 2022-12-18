#include "tools/StreamWriter.h"

namespace fisk::tools
{
	StreamWriter::StreamWriter(WriteStream& aStream)
		: myStream(aStream)
	{
	}

	bool StreamWriter::Process(uint8_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int8_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint16_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int16_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint32_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int32_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint64_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int64_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(std::string& aValue)
	{
		myStream.WriteData(reinterpret_cast<const uint8_t*>(aValue.c_str()), aValue.size() + 1);
		return true;
	}

	void StreamWriter::WriteRaw(uint8_t* aData, uint32_t aSize)
	{
		myStream.WriteData(aData, aSize);
	}

} // namespace fisk::tools