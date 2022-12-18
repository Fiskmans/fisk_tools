#ifndef FISK_TOOLS_STREAM_WRITER_H
#define FISK_TOOLS_STREAM_WRITER_H

#include "tools/DataProcessor.h"
#include "tools/Stream.h"

#include <memory>

namespace fisk::tools
{

	class StreamWriter : public DataProcessor
	{
	public:
		StreamWriter(WriteStream& aStream);

		bool Process(uint8_t& aValue) override;
		bool Process(int8_t& aValue) override;

		bool Process(uint16_t& aValue) override;
		bool Process(int16_t& aValue) override;

		bool Process(uint32_t& aValue) override;
		bool Process(int32_t& aValue) override;

		bool Process(uint64_t& aValue) override;
		bool Process(int64_t& aValue) override;

		bool Process(std::string& aValue) override;

	private:
		void WriteRaw(uint8_t* aData, uint32_t aSize);

		WriteStream& myStream;
	};
} // namespace fisk::tools

#endif