
#include <catch2/catch_all.hpp>

#include "tools/Iterators.h"

TEST_CASE("Iterators", "[Iterators]")
{
	{
		int arr[] = {1, 2, 3, 4, 5, 6};

		fisk::tools::LoopingPointer<int*> limited(arr, 3);

		REQUIRE(*limited == 1);
		limited++;
		REQUIRE(*limited == 2);
		limited++;
		REQUIRE(*limited == 3);
		limited++;
		REQUIRE(*limited == 1);
		limited++;
		REQUIRE(*limited == 2);
		limited++;
		REQUIRE(*limited == 3);
		limited++;
	}

	{
		int arr[] = {1, 2, 3, 4, 5, 6};

		fisk::tools::LoopingPointer<int*> limited(arr, 3);

		REQUIRE(*limited == 1);
		++limited;
		REQUIRE(*limited == 2);
		++limited;
		REQUIRE(*limited == 3);
		++limited;
		REQUIRE(*limited == 1);
		++limited;
		REQUIRE(*limited == 2);
		++limited;
		REQUIRE(*limited == 3);
		++limited;
	}

	{
		int arr[] = {1, 2, 3, 4, 5, 6};

		fisk::tools::LoopingPointer<int*> limited(arr, 1);

		REQUIRE(*limited == 1);
		limited++;
		REQUIRE(*limited == 1);
		limited++;
		REQUIRE(*limited == 1);
		limited++;
	}

	{
		int arr[] = { 1, 2, 3, 4, 5, 6 };
		fisk::tools::LoopingPointer<int*> limited(arr, 6);

		REQUIRE(*limited == 1);
		limited++;
		REQUIRE(*limited == 2);
		limited++;
		REQUIRE(*limited == 3);
		limited++;
		REQUIRE(*limited == 4);
		limited++;
		REQUIRE(*limited == 5);
		limited++;
		REQUIRE(*limited == 6);
		limited++;
		REQUIRE(*limited == 1);
		limited++;
		REQUIRE(*limited == 2);
		limited++;
		REQUIRE(*limited == 3);
		limited++;
		REQUIRE(*limited == 4);
		limited++;
		REQUIRE(*limited == 5);
		limited++;
		REQUIRE(*limited == 6);
		limited++;
	}

	{
		int arr[] = { 1, 2, 3, 4, 5, 6 };
		fisk::tools::LoopingPointer<int*> limited(arr, 6);

		REQUIRE(*limited == 1);
		++limited;
		REQUIRE(*limited == 2);
		++limited;
		REQUIRE(*limited == 3);
		++limited;
		REQUIRE(*limited == 4);
		++limited;
		REQUIRE(*limited == 5);
		++limited;
		REQUIRE(*limited == 6);
		++limited;
		REQUIRE(*limited == 1);
		++limited;
		REQUIRE(*limited == 2);
		++limited;
		REQUIRE(*limited == 3);
		++limited;
		REQUIRE(*limited == 4);
		++limited;
		REQUIRE(*limited == 5);
		++limited;
		REQUIRE(*limited == 6);
		++limited;
	}
}