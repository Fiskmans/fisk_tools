

#include <catch2/catch_all.hpp>

#include "tools/ObjectPool.h"

TEST_CASE("Object pool", "[Container]")
{
	{
		fisk::tools::ObjectPool<int> pool(16);

		int* a = pool.GetItem();
		REQUIRE(a);
		*a = 5;

		REQUIRE(*a == 5);

		int* b = pool.GetItem(3);
		REQUIRE(b);
		REQUIRE(*b == 3);
		REQUIRE(a != b);

		*b = 6;
		REQUIRE(*b == 6);
		REQUIRE(*a != 6);

		pool.ReturnItem(a);
		int* c = pool.GetItem();

		REQUIRE(c == a);

		pool.ReturnItem(b);
		pool.ReturnItem(c);
	}

	{
		fisk::tools::ObjectPool<int> pool2(2, false);

		int* d = pool2.GetItem();
		int* e = pool2.GetItem();

		REQUIRE(!pool2.GetItem());

		pool2.ReturnItem(d);

		int* f = pool2.GetItem();

		REQUIRE(f);
		REQUIRE(!pool2.GetItem());

		pool2.ReturnItem(e);
		pool2.ReturnItem(f);
	}
}
