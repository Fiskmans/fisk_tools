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
} // namespace fisk::tools

#endif