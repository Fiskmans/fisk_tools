
#include <catch2/catch_all.hpp>

#include "tools/ArenaAllocator.h"

TEST_CASE("ArenaAllocator", "[Memory]")
{
	{
		struct Foo
		{
			Foo(std::string aName)
			{
				myName = aName;
			}

			std::string myName;
		};

		fisk::tools::ArenaAllocator<Foo, 2> allocator;

		Foo* a = allocator.New("A");

		REQUIRE(a);
		REQUIRE(a->myName == "A");

		Foo* b = allocator.New("B");

		REQUIRE(b);
		REQUIRE(a->myName == "A");
		REQUIRE(b->myName == "B");
		REQUIRE(b == a + 1);

		Foo* c = allocator.New("C");

		REQUIRE(c);
		REQUIRE(a->myName == "A");
		REQUIRE(b->myName == "B");
		REQUIRE(c->myName == "C");
		REQUIRE(c != a + 1);
		REQUIRE(c != a);
		REQUIRE(c != b);

		allocator.Delete(a);

		REQUIRE(b->myName == "B");
		REQUIRE(c->myName == "C");

		allocator.Delete(b);

		Foo* d = allocator.New("D");
		REQUIRE(d);
		REQUIRE(d == c + 1);

		REQUIRE(c->myName == "C");

		allocator.Delete(c);
		allocator.Delete(d);
	}

	{
		struct Counter
		{
			Counter(int* aNumber)
			{
				(*aNumber)++;
				myNumber = aNumber;
			}
			~Counter()
			{
				(*myNumber)--;
			}

			int* myNumber;
		};

		fisk::tools::ArenaAllocator<Counter, 2> allocator;

		int alive = 0;

		Counter* a = allocator.New(&alive);

		REQUIRE(alive == 1);

		allocator.Delete(a);

		REQUIRE(alive == 0);

		a = allocator.New(&alive);
		Counter* b = allocator.New(&alive);

		REQUIRE(alive == 2);

		allocator.Delete(a);

		REQUIRE(alive == 1);

		allocator.Delete(b);

		REQUIRE(alive == 0);

	}
}