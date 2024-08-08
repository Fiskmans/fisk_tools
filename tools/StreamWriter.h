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

		bool Process(bool& aValue) override;

		bool Process(uint8_t& aValue) override;
		bool Process(int8_t& aValue) override;

		bool Process(uint16_t& aValue) override;
		bool Process(int16_t& aValue) override;

		bool Process(uint32_t& aValue) override;
		bool Process(int32_t& aValue) override;

		bool Process(uint64_t& aValue) override;
		bool Process(int64_t& aValue) override;

		bool Process(float& aValue) override;
		bool Process(double& aValue) override;

		bool Process(std::string& aValue) override;

		template<class T>
		bool Process(T& aValue)
		{
			return DataProcessor::Process(aValue);
		}

	private:
		void WriteRawNumeric(const uint8_t* aData, uint32_t aSize);

		void WriteRawReversed(const uint8_t* aData, uint32_t aSize);
		void WriteRaw(const uint8_t* aData, uint32_t aSize);

		WriteStream& myStream;
	};
} // namespace fisk::tools

#endif