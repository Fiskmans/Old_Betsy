#ifndef TOOLS_LITERALS_H
#define TOOLS_LITERALS_H

#include "tools/Types.h"

namespace tools 
{
	namespace size_literals
	{
		Distance operator""_m(long double aMeterValue);
		Distance operator""_dm(long double aDeciMeterValue);
		Distance operator""_cm(long double aCentiMeterValue);
		Distance operator""_m2(long double aMeterValue);
		Distance operator""_dm2(long double aDeciMeterValue);
		Distance operator""_cm2(long double aCentiMeterValue);
		Distance operator""_m3(long double aMeterValue);
		Distance operator""_dm3(long double aDeciMeterValue);
		Distance operator""_cm3(long double aCentiMeterValue);

		Distance operator""_m(size_t aMeterValue);
		Distance operator""_dm(size_t aDeciMeterValue);
		Distance operator""_cm(size_t aCentiMeterValue);
		Distance operator""_m2(size_t aMeterValue);
		Distance operator""_dm2(size_t aDeciMeterValue);
		Distance operator""_cm2(size_t aCentiMeterValue);
		Distance operator""_m3(size_t aMeterValue);
		Distance operator""_dm3(size_t aDeciMeterValue);
		Distance operator""_cm3(size_t aCentiMeterValue);
	}

	namespace byte_literals
	{
		size_t operator""_b(size_t aBytes);
		size_t operator""_kb(size_t aKiloBytes);
		size_t operator""_mb(size_t aMegaBytes);
		size_t operator""_gb(size_t aGigaBytes);
		size_t operator""_tb(size_t aTeraBytes);
	}

	namespace rotation_literals
	{
		Rotation operator""_deg(long double aValue);
		Rotation operator""_deg(size_t aValue);

		Rotation operator""_rad(long double aValue);
		Rotation operator""_rad(size_t aValue);
	}
}

#endif // !TOOLS_LITERALS_H
