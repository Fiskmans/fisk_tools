#ifndef FISK_TOOLS_STREAM_H
#define FISK_TOOLS_STREAM_H

#include "tools/ObjectPool.h"

#include <cstdint>
#include <memory>
#include <iostream>

namespace fisk::tools
{
	struct StreamSegment
	{
		static constexpr size_t CHUNK_SIZE = 1 << 10;

		size_t SpaceLeft();

		size_t Write(const uint8_t* aData, size_t aSize);
		size_t Read(uint8_t* aData, size_t aOffset, size_t aSize);

		size_t mySize = 0;
		std::shared_ptr<StreamSegment> myNext;

		uint8_t myData[CHUNK_SIZE] = {};
	};

	struct StreamOffset
	{
		std::shared_ptr<StreamSegment> mySegment;
		size_t myOffset = 0;

		operator bool();
	};

	class StreamIterator
	{
	public:
		using value_type = uint8_t;
		using difference_type = long long;

		StreamIterator();
		StreamIterator(const StreamOffset& aOffset);
		StreamIterator(StreamSegment* aSegment, long long aOffset);

		value_type& operator*();
		value_type& operator*() const;

		StreamIterator& operator++();
		StreamIterator operator++(int);

		bool operator==(const StreamIterator& aOther) const;

	private:
		StreamSegment* mySegment;
		long long myOffset;
	};

	class ReadStream
	{
	public:
		void AppendData(std::shared_ptr<StreamSegment> aData);
		bool AppendAllFromStream(std::istream& aStream);

		bool Read(uint8_t* aData, size_t aSize);
		size_t Peek(uint8_t* aData, size_t aSize);

		void CommitRead();
		void RestoreRead();

		RangeFromStartEnd<StreamIterator> AvailableData();

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
		bool WriteAllToStream(std::ostream& aStream);

		std::shared_ptr<StreamSegment> Get();
		bool HasData();

	private:

		std::shared_ptr<StreamSegment> myWriteHead;
		std::shared_ptr<StreamSegment> myHead;
	};

} // namespace fisk::tools

#endif