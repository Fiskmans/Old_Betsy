#include <pch.h>
#include "FastSqrt.h"

float FastInverseSquareRoot(const float aNumber) //Source: Quake III Arena
{
	static_assert(sizeof(long) == 4 && "Oh nooooooooo");

	if (aNumber == 0.f)
	{
		return 0.f;
	}

	const float threeHalves = 1.5f;

	const float x2 = aNumber * 0.5f;
	float y = aNumber;

	long i = *(long*)&y;					//evil floating point bit level hacking
	i = 0x5F375A86 - (i >> 1);				//what the fuck? 

	y = *(float*)&i;

	return (y * (threeHalves - (x2 * y * y)));
}


double FastInverseSquareRoot(const double aNumber)
{
	if (aNumber == 0)
	{
		return 0;
	}

	const double threeHalves = 1.5;

	const double x2 = aNumber * 0.5;
	double y = aNumber;

	long i = *(long*)&y;					//evil floating point bit level hacking
	i = 0x5FE6EB50C7B537A9 - (i >> 1);				//what the fuck? 

	y = *(double*)&i;

	return (y * (threeHalves - (x2 * y * y)));
}