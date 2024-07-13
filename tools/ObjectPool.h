#ifndef FISK_TOOLS_OBJECT_POOL_H
#define FISK_TOOLS_OBJECT_POOL_H

#include "tools/Iterators.h"

#include <cassert>
#include <stack>

namespace fisk::tools
{
	template <class T>
	class ObjectPool
	{
	public:
		ObjectPool(size_t aPoolSize, bool aAllowExcess = true);
		~ObjectPool();

		template <typename... Args>
		T* GetItem(Args... aArgs);

		void ReturnItem(T* aObject);

	private:
		using StorageType = std::aligned_storage<sizeof(T), alignof(T)>::type;

		T* myBegin;
		T* myEnd;

		std::stack<T*> myAvailable;
		bool myAllowExcess;
	};

	template <class T>
	inline ObjectPool<T>::ObjectPool(size_t aPoolSize, bool aAllowExcess)
	{
		myBegin		  = reinterpret_cast<T*>(new StorageType[aPoolSize]);
		myEnd		  = myBegin + aPoolSize;
		myAllowExcess = aAllowExcess;

		for (T& item : RangeFromStartEnd(myBegin, myEnd))
			myAvailable.push(&item);
	}

	template <class T>
	inline ObjectPool<T>::~ObjectPool()
	{
		assert(myAvailable.size() == std::distance(myBegin, myEnd) && "Memory leak");

		delete[] reinterpret_cast<StorageType*>(myBegin);
	}

	template <class T>
	template <typename... Args>
	inline T* ObjectPool<T>::GetItem(Args... aArgs)
	{
		if (myAvailable.empty())
		{
			if (!myAllowExcess)
				return nullptr;

			return new T(std::forward<Args>(aArgs)...);
		}

		T* ret = myAvailable.top();
		myAvailable.pop();

		new (ret) T(std::forward<Args>(aArgs)...);

		return ret;
	}

	template <class T>
	inline void ObjectPool<T>::ReturnItem(T* aObject)
	{
		if ((myBegin <= aObject) && (aObject < myEnd))
		{
			aObject->~T();
			myAvailable.push(aObject);
			return;
		}

		assert(myAllowExcess);
		delete aObject;
	}

} // namespace fisk::tools

#endif