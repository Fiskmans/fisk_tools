#include "tools/Stream.h"

#include <algorithm>

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

	uint8_t StreamSegment::Read(uint8_t* aData, size_t aOffset, size_t aSize)
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
		if (!myReadHead)
			return false;

		uint8_t* at = aData;
		size_t left = aSize;

		while (left > 0)
		{
			size_t amount = myReadHead.mySegment->Read(at, myReadHead.myOffset, left);

			left -= amount;
			at += amount;
			myReadHead.myOffset += amount;

			if (myReadHead.myOffset == myReadHead.mySegment->mySize)
			{
				std::shared_ptr<StreamSegment> old = myReadHead.mySegment;

				myReadHead.myOffset	 = 0;
				myReadHead.mySegment = myReadHead.mySegment->myNext;

				if (left > 0 && !myReadHead.mySegment)
					return false;
			}
		}

		return true;
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

} // namespace fisk::tools