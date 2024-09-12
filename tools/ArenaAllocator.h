#pragma once 

namespace fisk::tools
{
	template<class T, size_t PerBlock>
	class ArenaBlock
	{
	public:
		ArenaBlock() = default;
		~ArenaBlock() = default;

		ArenaBlock(ArenaBlock* aLastBlock);

		T* NextAlloc();

		ArenaBlock* GetLastBlock();

	private:
		ArenaBlock* myLastBlock = nullptr;
		size_t myAt = 0;

		std::aligned_storage_t<sizeof(T), alignof(T)> myBlock[PerBlock];
	};

	template<class T, size_t PerBlock>
	class ArenaAllocator
	{
		using BlockType = ArenaBlock<T, PerBlock>;

	public:
		ArenaAllocator();
		~ArenaAllocator();

		template<class... ConstructorArgs>
		T* New(ConstructorArgs&&... aConstructorArgs);
		void Delete(T* aObject);

	private:
		BlockType* myBlock;
	};


	/////////////////////////////////////////// ArenaBlock

	template<class T, size_t PerBlock>
	inline ArenaBlock<T, PerBlock>::ArenaBlock(ArenaBlock* aLastBlock)
	{
		myLastBlock = aLastBlock;
	}

	template<class T, size_t PerBlock>
	inline T* ArenaBlock<T, PerBlock>::NextAlloc()
	{
		if (myAt == PerBlock)
			return nullptr;

		return reinterpret_cast<T*>(&myBlock[myAt++]);
	}

	template<class T, size_t PerBlock>
	inline ArenaBlock<T, PerBlock>* ArenaBlock<T, PerBlock>::GetLastBlock()
	{
		return myLastBlock;
	}

	/////////////////////////////////////////// ArenaAllocator

	template<class T, size_t PerBlock>
	inline ArenaAllocator<T, PerBlock>::ArenaAllocator()
	{
		myBlock = new BlockType();
	}

	template<class T, size_t PerBlock>
	inline ArenaAllocator<T, PerBlock>::~ArenaAllocator()
	{
		while (myBlock)
		{
			BlockType* next = myBlock->GetLastBlock();
			delete myBlock;
			myBlock = next;
		}
	}

	template<class T, size_t PerBlock>
	template<class ...ConstructorArgs>
	inline T* ArenaAllocator<T, PerBlock>::New(ConstructorArgs && ...aConstructorArgs)
	{
		T* out = myBlock->NextAlloc();
		if (!out)
		{
			myBlock = new BlockType(myBlock);
			out = myBlock->NextAlloc();
		}

		std::construct_at(out, std::forward<ConstructorArgs>(aConstructorArgs)...);

		return out;
	}

	template<class T, size_t PerBlock>
	inline void ArenaAllocator<T, PerBlock>::Delete(T* aObject)
	{
		std::destroy_at(aObject);
	}

}