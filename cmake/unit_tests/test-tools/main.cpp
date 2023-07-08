
#include <ranges>

#include "catch2/catch_all.hpp"

#include "tools/FloatCompare.h"
#include "tools/MathVector.h"
#include "tools/StringManipulation.h"

TEST_CASE("MathVector", "[Math]")
{
	SECTION("Construction")
	{
		tools::MathVector<float, 2> fsize2(0.1f, 0.2f);
		tools::MathVector<float, 3> fsize3(0.1f, 0.2f, 0.3f);
		tools::MathVector<float, 6> fsize6(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f);

		tools::MathVector<double, 2> dsize2(0.1, 0.2);
		tools::MathVector<double, 3> dsize3(0.1, 0.2, 0.3);
		tools::MathVector<double, 6> dsize6(0.1, 0.2, 0.3, 0.4, 0.5, 0.6);

		tools::MathVector<int, 2> isize2(-1, 2);
		tools::MathVector<int, 3> isize3(-1, 2, 3);
		tools::MathVector<int, 6> isize6(-1, 2, 3, 4, 5, 6);

		tools::MathVector<unsigned char, 2> ucsize2(1, 2);
		tools::MathVector<unsigned char, 3> ucsize3(1, 2, 3);
		tools::MathVector<unsigned char, 6> ucsize6(1, 2, 3, 4, 5, 6);
	}

	SECTION("Compare")
	{
		tools::MathVector<float, 2> fsize2_1(0.1f, 0.2f);
		tools::MathVector<float, 2> fsize2_2(0.1f, 0.2f);
		tools::MathVector<float, 2> fsize2_3(0.1f, 0.3f);
		tools::MathVector<float, 2> fsize2_4(0.2f, 0.2f);

		REQUIRE(fsize2_1 == fsize2_2);
		REQUIRE(!(fsize2_1 != fsize2_2));

		REQUIRE(!(fsize2_1 == fsize2_3));
		REQUIRE((fsize2_1 != fsize2_3));

		REQUIRE(!(fsize2_1 == fsize2_4));
		REQUIRE((fsize2_1 != fsize2_4));

		tools::MathVector<int, 2> isize2_1(1, 2);
		tools::MathVector<int, 2> isize2_2(1, 2);
		tools::MathVector<int, 2> isize2_3(1, 3);
		tools::MathVector<int, 2> isize2_4(2, 2);

		REQUIRE(isize2_1 == isize2_2);
		REQUIRE(!(isize2_1 != isize2_2));

		REQUIRE(!(isize2_1 == isize2_3));
		REQUIRE((isize2_1 != isize2_3));

		REQUIRE(!(isize2_1 == isize2_4));
		REQUIRE((isize2_1 != isize2_4));
	}

	SECTION("Content")
	{
		tools::MathVector<float, 2> fsize2(0.1f, 0.2f);
		REQUIRE(fsize2[0] == 0.1f);
		REQUIRE(fsize2[1] == 0.2f);

		tools::MathVector<int, 3> isize3(-1, 2, 3);
		REQUIRE(isize3[0] == -1);
		REQUIRE(isize3[1] == 2);
		REQUIRE(isize3[2] == 3);

		tools::MathVector<unsigned char, 6> ucsize6(1, 2, 3, 4, 5, 6);
		REQUIRE(ucsize6[0] == 1);
		REQUIRE(ucsize6[1] == 2);
		REQUIRE(ucsize6[2] == 3);
		REQUIRE(ucsize6[3] == 4);
		REQUIRE(ucsize6[4] == 5);
		REQUIRE(ucsize6[5] == 6);
	}

	SECTION("Length")
	{
		tools::MathVector<int, 2> isize2_1(3, 4);
		tools::MathVector<int, 2> isize2_2(-3, 4);
		tools::MathVector<int, 2> isize2_3(3, -4);
		tools::MathVector<int, 2> isize2_4(-3, -4);

		tools::MathVector<int, 2> isize2_5(6, 8);

		tools::MathVector<int, 3> isize3_1(2, 3, 6);
		tools::MathVector<int, 3> isize3_2(3, 2, 6);
		tools::MathVector<int, 3> isize3_3(6, 3, 2);
		tools::MathVector<int, 3> isize3_4(2, 6, 3);

		REQUIRE(isize2_1.LengthSqr() == 25);
		REQUIRE(isize2_2.LengthSqr() == 25);
		REQUIRE(isize2_3.LengthSqr() == 25);
		REQUIRE(isize2_4.LengthSqr() == 25);

		REQUIRE(isize2_1.Length() == 5);
		REQUIRE(isize2_2.Length() == 5);
		REQUIRE(isize2_3.Length() == 5);
		REQUIRE(isize2_4.Length() == 5);

		REQUIRE(isize2_5.LengthSqr() == 100);
		REQUIRE(isize2_5.Length() == 10);

		REQUIRE(isize3_1.LengthSqr() == 49);
		REQUIRE(isize3_2.LengthSqr() == 49);
		REQUIRE(isize3_3.LengthSqr() == 49);
		REQUIRE(isize3_4.LengthSqr() == 49);

		REQUIRE(isize3_1.Length() == 7);
		REQUIRE(isize3_2.Length() == 7);
		REQUIRE(isize3_3.Length() == 7);
		REQUIRE(isize3_4.Length() == 7);
	}

	SECTION("math")
	{
		//PLUS
		tools::MathVector<int, 2> isize2_1(3, 4);
		tools::MathVector<int, 2> isize2_2(1, 2);

		tools::MathVector<int, 2> isize2_3(5, 2);
		tools::MathVector<int, 2> isize2_4 = isize2_1 + isize2_2;

		isize2_3 += isize2_2;

		REQUIRE(isize2_3[0] == 6);
		REQUIRE(isize2_3[1] == 4);

		REQUIRE(isize2_4[0] == 4);
		REQUIRE(isize2_4[1] == 6);

		//MINUS
		tools::MathVector<int, 5> isize5_1(-2, -1, 0, 1, 2);
		tools::MathVector<int, 5> isize5_2(1, 2, 3, 4, 5);
		
		tools::MathVector<int, 5> isize5_3(isize5_1);
		tools::MathVector<int, 5> isize5_4 = isize5_1 + isize5_2;

		isize5_3 += isize5_2;

		REQUIRE(isize5_3[0] == -1);
		REQUIRE(isize5_3[1] == 1);
		REQUIRE(isize5_3[2] == 3);
		REQUIRE(isize5_3[3] == 5);
		REQUIRE(isize5_3[4] == 7);

		REQUIRE(isize5_4[0] == -1);
		REQUIRE(isize5_4[1] == 1);
		REQUIRE(isize5_4[2] == 3);
		REQUIRE(isize5_4[3] == 5);
		REQUIRE(isize5_4[4] == 7);

		//MUL
		tools::MathVector<int, 3> isize3_1(1, 2, -3);

		tools::MathVector<int, 3> isize3_2(1, -2, 3);
		tools::MathVector<int, 3> isize3_3 = isize3_1 * -3;

		isize3_2 *= 2;

		REQUIRE(isize3_2[0] == 2);
		REQUIRE(isize3_2[1] == -4);
		REQUIRE(isize3_2[2] == 6);

		REQUIRE(isize3_3[0] == -3);
		REQUIRE(isize3_3[1] == -6);
		REQUIRE(isize3_3[2] == 9);

		//DIV
		tools::MathVector<int, 3> isize3_4(-3, -6, 9);

		tools::MathVector<int, 3> isize3_5(2, -4, 6);
		tools::MathVector<int, 3> isize3_6 = isize3_4 / -3;

		isize3_5 /= 2;

		REQUIRE(isize3_5[0] == 1);
		REQUIRE(isize3_5[1] == -2);
		REQUIRE(isize3_5[2] == 3);

		REQUIRE(isize3_6[0] == 1);
		REQUIRE(isize3_6[1] == 2);
		REQUIRE(isize3_6[2] == -3);

		//DOT
		tools::MathVector<int, 4> isize4_1(1, 2, 3, 4);

		tools::MathVector<int, 4> isize4_2(1, 0, 0, 0);
		tools::MathVector<int, 4> isize4_3(0, 1, 0, 0);
		tools::MathVector<int, 4> isize4_4(0, 0, 1, 0);
		tools::MathVector<int, 4> isize4_5(0, 0, 0, 1);

		tools::MathVector<int, 4> isize4_6(5, 6, 7, 8);
		tools::MathVector<int, 4> isize4_7(5, -6, -7, 8);

		REQUIRE(isize4_1.Dot(isize4_2) == 1);
		REQUIRE(isize4_1.Dot(isize4_3) == 2);
		REQUIRE(isize4_1.Dot(isize4_4) == 3);
		REQUIRE(isize4_1.Dot(isize4_5) == 4);

		REQUIRE(isize4_1.Dot(isize4_6) == (5 + 12 + 21 + 32));
		REQUIRE(isize4_1.Dot(isize4_7) == (5 - 12 - 21 + 32));

		//NORMAL

		tools::MathVector<float, 2> fsize2_1(3.f, 4.f);
		tools::MathVector<float, 2> fsize2_2(3.f, 4.f);
		tools::MathVector<float, 2> fsize2_3 = fsize2_1.GetNormalized();

		fsize2_2.Normalize();

		REQUIRE(abs(fsize2_2[0] - (3.f / 5.f)) < 1e-10f);
		REQUIRE(abs(fsize2_2[1] - (4.f / 5.f)) < 1e-10f);

		REQUIRE(abs(fsize2_3[0] - (3.f / 5.f)) < 1e-10f);
		REQUIRE(abs(fsize2_3[1] - (4.f / 5.f)) < 1e-10f);


		//REFLECT
		tools::MathVector<double, 2> dsize2_1(1.0, 0.1);

		tools::MathVector<double, 2> dsize2_2(1.0, 1.0);

		tools::MathVector<double, 2> dsize2_3(1.0, 0.1);
		tools::MathVector<double, 2> dsize2_4 = dsize2_1.Reflected(dsize2_2);

		dsize2_3.Reflect(dsize2_2);

		REQUIRE(abs(dsize2_3[0] - 0.1) < 1e-10f);
		REQUIRE(abs(dsize2_3[1] - 1.0) < 1e-10f);

		REQUIRE(abs(dsize2_4[0] - 0.1) < 1e-10f);
		REQUIRE(abs(dsize2_4[1] - 1.0) < 1e-10f);
	}

	SECTION("Extend")
	{
		tools::MathVector<float, 2> fsize2(1.f, 2.f);
		
		tools::MathVector<float, 3> fsize3_1 = fsize2.Extend(3.f);
		tools::MathVector<float, 3> fsize3_2 = fsize2.Extend<float>(3.f);
		
		tools::MathVector<float, 4> fsize4_1 = fsize2.Extend(3.f, 4.f);
		tools::MathVector<float, 4> fsize4_2 = fsize2.Extend<float, float>(3.f, 4.f);

		REQUIRE(fsize3_1[0] == 1.f);
		REQUIRE(fsize3_1[1] == 2.f);
		REQUIRE(fsize3_1[2] == 3.f);

		REQUIRE(fsize3_2[0] == 1.f);
		REQUIRE(fsize3_2[1] == 2.f);
		REQUIRE(fsize3_2[2] == 3.f);

		REQUIRE(fsize4_1[0] == 1.f);
		REQUIRE(fsize4_1[1] == 2.f);
		REQUIRE(fsize4_1[2] == 3.f);
		REQUIRE(fsize4_1[3] == 4.f);

		REQUIRE(fsize4_2[0] == 1.f);
		REQUIRE(fsize4_2[1] == 2.f);
		REQUIRE(fsize4_2[2] == 3.f);
		REQUIRE(fsize4_2[3] == 4.f);
	}
}

TEST_CASE("String manipulation")
{
	REQUIRE(tools::PathWithoutFile("c:/test/blah/file.exe") == "c:/test/blah/");
	REQUIRE(tools::ExtensionFromPath("c:/test/blah/file.exe") == ".exe");
}