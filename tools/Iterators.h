#ifndef FISK_TOOLS_ITERATORS_H
#define FISK_TOOLS_ITERATORS_H

#include <iterator>

namespace fisk::tools
{
	template <std::forward_iterator T> 
	class RangeFromStartEnd
	{
	public:
		RangeFromStartEnd(T aBegin, T aEnd)
			: myBegin(aBegin)
			, myEnd(aEnd)
		{
		}

		T begin()
		{
			return myBegin;
		}

		T end()
		{
			return myEnd;
		}

	private:
		T myBegin;
		T myEnd;
	};


	template<class Type>
	class LoopingPointer
	{
	public:
		LoopingPointer(Type* aBase, size_t aSize)
			: myBase(aBase)
			, mySize(aSize)
			, myAt(0)
		{

		}

		void operator++()
		{
			++myAt %= mySize;
		}
		void operator++(int)
		{
			++myAt %= mySize;
		}

		bool operator==(const LoopingPointer& aOther)
		{
			return myAt == aOther.myAt;
		}

		Type& operator*()
		{
			return myBase[myAt];
		}
		Type* operator->()
		{
			return myBase + myAt;
		}

	private:

		Type* myBase;
		size_t mySize;
		size_t myAt;
	};

} // namespace fisk::tools

#endif