#include "pch.h"
#include "Literals.h"

float operator""_m(long double aMeterValue)
{
	return static_cast<float>(aMeterValue * globalMeterScaleFactor);
}

float operator""_cm(long double aCentiMeterValue)
{
	return static_cast<float>(aCentiMeterValue);
}

float operator""_m(size_t aMeterValue)
{
	return static_cast<float>(aMeterValue * globalMeterScaleFactor);
}

float operator""_cm(size_t aCentiMeterValue)
{
	return static_cast<float>(aCentiMeterValue);
}
