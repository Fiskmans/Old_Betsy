#include "pch.h"
#include "Literals.h"

float operator""_m(long double aMeterValue)
{
	return aMeterValue * globalMeterScaleFactor;
}

float operator""_cm(long double aCentiMeterValue)
{
	return aCentiMeterValue;
}

float operator""_m(size_t aMeterValue)
{
	return aMeterValue * globalMeterScaleFactor;
}

float operator""_cm(size_t aCentiMeterValue)
{
	return aCentiMeterValue;
}
