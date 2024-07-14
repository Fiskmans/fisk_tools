
#include <catch2/catch_all.hpp>

#include "tools/MathVector.h"
#include "tools/Matrix.h"

TEST_CASE("Vector", "[Math]")
{
	{
		fisk::tools::MathVector<int, 3> vec3(1, 2, 3);

		REQUIRE(vec3[0] == 1);
		REQUIRE(vec3[1] == 2);
		REQUIRE(vec3[2] == 3);
	}

	{
		fisk::tools::MathVector<int, 3> a(1, 2, 3);
		fisk::tools::MathVector<int, 3> b(5, 7, 11);

		REQUIRE(a.Dot(b) == (1 * 5 + 2 * 7 + 3 * 11));
	}

	{
		fisk::tools::MathVector<int, 3> a(1, 2, 3);
		fisk::tools::MathVector<int, 3> b(3, 2, 1);

		fisk::tools::MathVector<int, 3> c = a.Max(b);
		fisk::tools::MathVector<int, 3> d = a.Min(b);

		REQUIRE(c[0] == 3);
		REQUIRE(c[1] == 2);
		REQUIRE(c[2] == 3);

		REQUIRE(d[0] == 1);
		REQUIRE(d[1] == 2);
		REQUIRE(d[2] == 1);
	}
}

TEST_CASE("Matrix", "[Math]")
{
	{
		fisk::tools::Matrix<int, 2, 3> matrix(1, 2, 3, 4, 5, 6);

		REQUIRE(matrix.GetElement(0, 0) == 1);
		REQUIRE(matrix.GetElement(1, 0) == 2);
		REQUIRE(matrix.GetElement(0, 1) == 3);
		REQUIRE(matrix.GetElement(1, 1) == 4);
		REQUIRE(matrix.GetElement(0, 2) == 5);
		REQUIRE(matrix.GetElement(1, 2) == 6);

		int* raw = matrix.Raw();

		REQUIRE(raw[0] == 1);
		REQUIRE(raw[1] == 2);
		REQUIRE(raw[2] == 3);
		REQUIRE(raw[3] == 4);
		REQUIRE(raw[4] == 5);
		REQUIRE(raw[5] == 6);
	}

	{
		// clang-format off
		fisk::tools::Matrix<int, 4, 2> a(
			3, 2, 1, 5, 
			9, 1, 3, 0);

		fisk::tools::Matrix<int, 3, 4> b(
			2, 9, 0,
			1, 3, 5,
			2, 4, 7,
			8, 1, 5);

		// clang-format on

		fisk::tools::Matrix<int, 3, 2> result = a * b;

		REQUIRE(result.GetElement(0, 0) == 50);
		REQUIRE(result.GetElement(1, 0) == 42);
		REQUIRE(result.GetElement(2, 0) == 42);

		REQUIRE(result.GetElement(0, 1) == 25);
		REQUIRE(result.GetElement(1, 1) == 96);
		REQUIRE(result.GetElement(2, 1) == 26);
	}

	{

		using mi33 = fisk::tools::Matrix<int, 3, 3>;
		// clang-format off
		mi33 base(
			1, 2, 3, 
			3, 4, 2, 
			3, 2, 1);
		
		mi33 second(
			1, 1, 1, 
			3, 4, 2, 
			3, 2, 1);
		// clang-format on


		mi33& ref = (base *= second);

		REQUIRE(&ref == &base);


		REQUIRE(ref.GetElement(0, 0) == 16);
		REQUIRE(ref.GetElement(1, 0) == 15);
		REQUIRE(ref.GetElement(2, 0) == 8);

		REQUIRE(ref.GetElement(0, 1) == 21);
		REQUIRE(ref.GetElement(1, 1) == 23);
		REQUIRE(ref.GetElement(2, 1) == 13);
		
		REQUIRE(ref.GetElement(0, 2) == 12);
		REQUIRE(ref.GetElement(1, 2) == 13);
		REQUIRE(ref.GetElement(2, 2) == 8);
	}
}