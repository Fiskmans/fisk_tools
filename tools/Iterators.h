#ifndef FISK_TOOLS_ITERATORS_H
#define FISK_TOOLS_ITERATORS_H

namespace fisk::tools
{
	template <class T> class IteratePointers
	{
	public:
		IteratePointers(T* aBegin, T* aEnd)
			: myBegin(aBegin)
			, myEnd(aEnd)
		{
		}

		T* begin()
		{
			return myBegin;
		}

		T* end()
		{
			return myEnd;
		}

	private:
		T* myBegin;
		T* myEnd;
	};
} // namespace fisk::tools

#endif