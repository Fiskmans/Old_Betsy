#pragma once

namespace tools 
{
	namespace size_literals
	{
		float operator""_m(long double aMeterValue);
		float operator""_dm(long double aDeciMeterValue);
		float operator""_cm(long double aCentiMeterValue);
		float operator""_m2(long double aMeterValue);
		float operator""_dm2(long double aDeciMeterValue);
		float operator""_cm2(long double aCentiMeterValue);
		float operator""_m3(long double aMeterValue);
		float operator""_dm3(long double aDeciMeterValue);
		float operator""_cm3(long double aCentiMeterValue);

		float operator""_m(size_t aMeterValue);
		float operator""_dm(size_t aDeciMeterValue);
		float operator""_cm(size_t aCentiMeterValue);
		float operator""_m2(size_t aMeterValue);
		float operator""_dm2(size_t aDeciMeterValue);
		float operator""_cm2(size_t aCentiMeterValue);
		float operator""_m3(size_t aMeterValue);
		float operator""_dm3(size_t aDeciMeterValue);
		float operator""_cm3(size_t aCentiMeterValue);
	}

	namespace byte_literals
	{
		size_t operator""_b(size_t aBytes);
		size_t operator""_kb(size_t aKiloBytes);
		size_t operator""_mb(size_t aMegaBytes);
		size_t operator""_gb(size_t aGigaBytes);
		size_t operator""_tb(size_t aTeraBytes);
	}
}