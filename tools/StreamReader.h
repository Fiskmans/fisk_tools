#ifndef FISK_TOOLS_STREAM_READER_H
#define FISK_TOOLS_STREAM_READER_H

#include "tools/Stream.h"
#include "tools/DataProcessor.h"

namespace fisk::tools
{

	class StreamReader : public DataProcessor
	{
	public:
		StreamReader(ReadStream& aReadStream);

		bool Process(uint8_t& aValue) override;
		bool Process(int8_t& aValue) override;

		bool Process(uint16_t& aValue) override;
		bool Process(int16_t& aValue) override;

		bool Process(uint32_t& aValue) override;
		bool Process(int32_t& aValue) override;

		bool Process(uint64_t& aValue) override;
		bool Process(int64_t& aValue) override;

	private:
		bool ReadRaw(uint8_t* aData, uint32_t aSize);

		ReadStream& myReadStream;
	};
} // namespace fisk::tools

#endif