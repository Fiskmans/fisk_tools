#include "tools/StreamWriter.h"

namespace fisk::tools
{
	StreamWriter::StreamWriter(WriteStream& aStream)
		: myStream(aStream)
	{
	}

	bool StreamWriter::Process(uint8_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int8_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint16_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int16_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint32_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int32_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint64_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int64_t& aValue)
	{
		WriteRawNumeric(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(std::string& aValue)
	{
		myStream.WriteData(reinterpret_cast<const uint8_t*>(aValue.c_str()), aValue.size() + 1);
		return true;
	}

	void StreamWriter::WriteRawNumeric(const uint8_t* aData, uint32_t aSize)
	{
		if constexpr (std::endian::native == std::endian::big)
			return WriteRaw(aData, aSize);
		else if constexpr (std::endian::native == std::endian::little)
			return WriteRawReversed(aData, aSize);
	}

	void StreamWriter::WriteRawReversed(const uint8_t* aData, uint32_t aSize)
	{
		uint8_t* rev = static_cast<uint8_t*>(alloca(aSize));
		memcpy(rev, aData, aSize);
		std::reverse(rev, rev + aSize);

		myStream.WriteData(rev, aSize);
	}
	void StreamWriter::WriteRaw(const uint8_t* aData, uint32_t aSize)
	{
		myStream.WriteData(aData, aSize);
	}

} // namespace fisk::tools