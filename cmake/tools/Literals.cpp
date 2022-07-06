#include "tools/Literals.h"

const float globalMeterScaleFactor = 100.f;
namespace tools
{
	namespace size_literals
	{

		float operator""_m(long double aMeterValue)
		{
			return static_cast<float>(aMeterValue * globalMeterScaleFactor);
		}

		float operator""_dm(long double aDeciMeterValue)
		{
			return static_cast<float>(aDeciMeterValue * globalMeterScaleFactor * 0.1);
		}

		float operator""_cm(long double aCentiMeterValue)
		{
			return static_cast<float>(aCentiMeterValue);
		}

		float operator""_m2(long double aMeterValue)
		{
			return static_cast<float>(aMeterValue) * 1_m * 1_m;
		}

		float operator""_dm2(long double aDeciMeterValue)
		{
			return static_cast<float>(aDeciMeterValue) * 1_dm * 1_dm;
		}

		float operator""_cm2(long double aCentiMeterValue)
		{
			return static_cast<float>(aCentiMeterValue) * 1_cm * 1_cm;
		}

		float operator""_m3(long double aMeterValue)
		{
			return static_cast<float>(aMeterValue) * 1_m * 1_m * 1_m;
		}

		float operator""_dm3(long double aDeciMeterValue)
		{
			return static_cast<float>(aDeciMeterValue) * 1_dm * 1_dm * 1_dm;
		}

		float operator""_cm3(long double aCentiMeterValue)
		{
			return static_cast<float>(aCentiMeterValue) * 1_cm * 1_cm * 1_cm;
		}

		float operator""_m(size_t aMeterValue)
		{
			return static_cast<float>(aMeterValue * globalMeterScaleFactor);
		}

		float operator""_dm(size_t aDeciMeterValue)
		{
			return static_cast<float>(aDeciMeterValue * globalMeterScaleFactor * 0.1);
		}

		float operator""_cm(size_t aCentiMeterValue)
		{
			return static_cast<float>(aCentiMeterValue);
		}

		float operator""_m2(size_t aMeterValue)
		{
			return static_cast<float>(aMeterValue) * 1_m * 1_m;
		}

		float operator""_dm2(size_t aDeciMeterValue)
		{
			return static_cast<float>(aDeciMeterValue) * 1_dm * 1_dm;
		}

		float operator""_cm2(size_t aCentiMeterValue)
		{
			return static_cast<float>(aCentiMeterValue) * 1_cm * 1_cm;
		}

		float operator""_m3(size_t aMeterValue)
		{
			return static_cast<float>(aMeterValue) * 1_m * 1_m * 1_m;
		}

		float operator""_dm3(size_t aDeciMeterValue)
		{
			return static_cast<float>(aDeciMeterValue) * 1_dm * 1_dm * 1_dm;
		}

		float operator""_cm3(size_t aCentiMeterValue)
		{
			return static_cast<float>(aCentiMeterValue) * 1_cm * 1_cm * 1_cm;
		}
	}

	namespace byte_literals
	{

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
	}

}