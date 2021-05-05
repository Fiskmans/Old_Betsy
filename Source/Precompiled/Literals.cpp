#include "pch.h"
#include "Literals.h"

float operator""m(long double aMeterValue)
{
	return aMeterValue * globalMeterScaleFactor;
}

float operator""cm(long double aCentiMeterValue)
{
	return aCentiMeterValue;
}

float operator""m(size_t aMeterValue)
{
	return aMeterValue * globalMeterScaleFactor;
}

float operator""cm(size_t aCentiMeterValue)
{
	return aCentiMeterValue;
}
