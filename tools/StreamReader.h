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

		template<class T>
		bool ProcessAndCommit(T& aValue)
		{
			if (Process(aValue))
			{
				myReadStream.CommitRead();
				return true;
			}

			myReadStream.RestoreRead();
			return false;
		}

	private:

		bool ReadRawNumeric(uint8_t* aData, uint32_t aSize);

		bool ReadRawReversed(uint8_t* aData, uint32_t aSize);
		bool ReadRaw(uint8_t* aData, uint32_t aSize);

		ReadStream& myReadStream;
	};
} // namespace fisk::tools

#endif