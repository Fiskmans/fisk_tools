#ifndef FISK_TOOLS_STREAM_H
#define FISK_TOOLS_STREAM_H

#include "tools/ObjectPool.h"

#include <cstdint>
#include <memory>

namespace fisk::tools
{
	struct StreamSegment
	{
		static constexpr size_t CHUNK_SIZE = 1 << 10;

		size_t SpaceLeft();

		size_t Write(const uint8_t* aData, size_t aSize);
		uint8_t Read(uint8_t* aData, size_t aOffset, size_t aSize);

		size_t mySize = 0;
		std::shared_ptr<StreamSegment> myNext;

		uint8_t myData[CHUNK_SIZE];
	};

	struct StreamOffset
	{
		std::shared_ptr<StreamSegment> mySegment;
		size_t myOffset = 0;

		operator bool();
	};

	class ReadStream
	{
	public:
		void AppendData(std::shared_ptr<StreamSegment> aData);

		bool Read(uint8_t* aData, size_t aSize);
		size_t Peek(uint8_t* aData, size_t aSize);

		void CommitRead();
		void RestoreRead();

	private:
		size_t PrivPeek(uint8_t* aData, size_t aSize, StreamOffset& aOutEnd);

		StreamOffset myReadHead;
		StreamOffset myCheckpoint;

		std::shared_ptr<StreamSegment> myTail;
	};

	class WriteStream
	{
	public:
		void WriteData(const uint8_t* aData, size_t aSize);

		std::shared_ptr<StreamSegment> Get();

	private:

		std::shared_ptr<StreamSegment> myWriteHead;
		std::shared_ptr<StreamSegment> myHead;
	};

} // namespace fisk::tools

#endif