#include "tools/Stream.h"

#include <algorithm>
#include <memory>
#include <cstring>

namespace fisk::tools
{

	size_t StreamSegment::SpaceLeft()
	{
		return CHUNK_SIZE - mySize;
	}

	size_t StreamSegment::Write(const uint8_t* aData, size_t aSize)
	{
		size_t amount = std::min(SpaceLeft(), aSize);

		memcpy(myData + mySize, aData, amount);
		mySize += amount;

		return amount;
	}

	size_t StreamSegment::Read(uint8_t* aData, size_t aOffset, size_t aSize)
	{
		size_t amount = std::min(mySize - aOffset, aSize);

		memcpy(aData, myData + aOffset, amount);

		return amount;
	}

	StreamOffset::operator bool()
	{
		return (bool)(mySegment);
	}

	void ReadStream::AppendData(std::shared_ptr<StreamSegment> aData)
	{
		if (myTail)
		{
			myTail->myNext = aData;
		}
		else
		{
			myTail				 = aData;
			myReadHead.mySegment = aData;
			myReadHead.myOffset	 = 0;

			myCheckpoint.mySegment = aData;
			myCheckpoint.myOffset  = 0;
		}

		while (myTail->myNext)
			myTail = myTail->myNext;
	}

	bool ReadStream::Read(uint8_t* aData, size_t aSize)
	{
		StreamOffset off;
		size_t amount = PrivPeek(aData, aSize, off);
		myReadHead	  = off;

		return amount == aSize;
	}

	size_t ReadStream::Peek(uint8_t* aData, size_t aSize)
	{
		StreamOffset off;

		return PrivPeek(aData, aSize, off);
	}

	void ReadStream::CommitRead()
	{
		while (myCheckpoint.mySegment != myReadHead.mySegment)
		{
			std::shared_ptr<StreamSegment> next = myCheckpoint.mySegment->myNext;

			myCheckpoint.mySegment = next;
		}

		myCheckpoint.myOffset = myReadHead.myOffset;
	}

	void ReadStream::RestoreRead()
	{
		myReadHead = myCheckpoint;
	}

	RangeFromStartEnd<StreamIterator> ReadStream::AvailableData()
	{
		return RangeFromStartEnd<StreamIterator>(myReadHead, StreamIterator(myTail.get(), myTail->mySize));
	}

	size_t ReadStream::PrivPeek(uint8_t* aData, size_t aSize, StreamOffset& aOutEnd)
	{
		if (!myReadHead)
			return 0;

		aOutEnd = myReadHead;

		uint8_t* at = aData;
		size_t left = aSize;

		while (left > 0)
		{
			if (aOutEnd.myOffset == aOutEnd.mySegment->mySize)
			{
				if (!aOutEnd.mySegment->myNext)
					break;

				std::shared_ptr<StreamSegment> old = aOutEnd.mySegment;

				aOutEnd.myOffset  = 0;
				aOutEnd.mySegment = aOutEnd.mySegment->myNext;
			}

			size_t amount = aOutEnd.mySegment->Read(at, aOutEnd.myOffset, left);

			left -= amount;
			at += amount;
			aOutEnd.myOffset += amount;
		}

		return aSize - left;
	}

	void WriteStream::WriteData(const uint8_t* aData, size_t aSize)
	{
		const uint8_t* at = aData;
		size_t left		  = aSize;

		while (left > 0)
		{
			if (!myWriteHead)
			{
				myHead = std::make_shared<StreamSegment>();

				myWriteHead = myHead;
			}

			size_t amount = myWriteHead->Write(at, left);

			at += amount;
			left -= amount;

			if (myWriteHead->SpaceLeft() == 0)
			{
				myWriteHead->myNext = std::make_shared<StreamSegment>();
				myWriteHead			= myWriteHead->myNext;
			}
		}
	}

	std::shared_ptr<StreamSegment> WriteStream::Get()
	{
		if (!myWriteHead)
			return nullptr;

		if (myWriteHead->mySize != 0)
		{
			myWriteHead = nullptr;
		}
		else
		{
			// find tail, detach, return
			std::shared_ptr<StreamSegment> at = myHead;
			std::shared_ptr<StreamSegment> last;
			while (at != myWriteHead)
			{
				last = at;
				at	 = at->myNext;
			}
			last->myNext = nullptr;
		}

		std::shared_ptr<StreamSegment> out = myHead;
		myHead							   = myWriteHead;

		return out;
	}

	bool WriteStream::HasData()
	{
		if (!myHead)
			return false;

		if (myHead->mySize == 0)
			return false;

		return true;
	}

	StreamIterator::StreamIterator()
		: mySegment(nullptr)
		, myOffset{}
	{
	}

	StreamIterator::StreamIterator(const StreamOffset& aOffset)
		: mySegment(aOffset.mySegment.get())
		, myOffset(aOffset.myOffset)
	{
	}

	StreamIterator::StreamIterator(StreamSegment* aSegment, long long aOffset)
		: mySegment(aSegment)
		, myOffset(aOffset)
	{
	}

	StreamIterator::value_type& StreamIterator::operator*()
	{
		return mySegment->myData[myOffset];
	}

	StreamIterator::value_type& StreamIterator::operator*() const
	{
		return mySegment->myData[myOffset];
	}

	StreamIterator& StreamIterator::operator++()
	{
		if (myOffset == mySegment->mySize)
		{
			mySegment = mySegment->myNext.get();
			myOffset = 0;
		}

		myOffset++;

		return *this;
	}

	StreamIterator StreamIterator::operator++(int)
	{
		StreamIterator copy(*this);
		++*this;
		return copy;
	}

	bool StreamIterator::operator==(const StreamIterator& aOther) const
	{
		if (mySegment != aOther.mySegment)
			return false;

		return myOffset == aOther.myOffset;
	}

} // namespace fisk::tools