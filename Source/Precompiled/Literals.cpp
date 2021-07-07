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

float operator""_m2(long double aMeterValue)
{
	return static_cast<float>(aMeterValue) * 1_m * 1_m;
}

float operator""_cm2(long double aCentiMeterValue)
{
	return static_cast<float>(aCentiMeterValue) * 1_cm * 1_cm;
}

float operator""_m(size_t aMeterValue)
{
	return static_cast<float>(aMeterValue * globalMeterScaleFactor);
}

float operator""_cm(size_t aCentiMeterValue)
{
	return static_cast<float>(aCentiMeterValue);
}

float operator""_m2(size_t aMeterValue)
{
	return static_cast<float>(aMeterValue) * 1_m * 1_m;
}

float operator""_cm2(size_t aCentiMeterValue)
{
	return static_cast<float>(aCentiMeterValue) * 1_cm * 1_cm;
}

size_t operator""_b(size_t aBytes)
{
	return aBytes;
}

size_t operator""_kb(size_t aKiloBytes)
{
	return aKiloBytes * (1ull << 10);
}

size_t operator""_mb(size_t aMegaBytes)
{
	return aMegaBytes * (1ull << 20);
}

size_t operator""_gb(size_t aGigaBytes)
{
	return aGigaBytes * (1ull << 30);
}

size_t operator""_tb(size_t aTeraBytes)
{
	return aTeraBytes * (1ull << 40);
}
