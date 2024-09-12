#include "tools/ArenaAllocator.h"

#include <type_traits>
#include <memory>
#include <cassert>

namespace fisk::tools
{
	ArenaBlock::ArenaBlock(size_t aSize)
	{
		static_assert(std::is_trivial_v<uint8_t>);

		myBlock = ::malloc(aSize);
		myAt = reinterpret_cast<uint8_t*>(myBlock);
		myEnd = myAt + aSize;
	}

	ArenaBlock::~ArenaBlock()
	{
		::free(myBlock);
	}

	void* ArenaBlock::Next(size_t aAmount)
	{
		if (myAt == myEnd)
			return nullptr;

		void* out = myAt;
		myAt += aAmount;
		return out;
	}

	ArenaBlock* ArenaBlock::GetLastBlock()
	{
		return myLast;
	}
}

