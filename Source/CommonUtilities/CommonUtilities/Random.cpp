#include "Random.hpp"

namespace CommonUtilities
{
	int RandomInt(int aMin, int aMax)
	{
		static std::random_device rndDevice;
		static std::mt19937 mt(rndDevice());

		std::uniform_int_distribution <int> rndDist(aMin, aMax);

		return rndDist(mt);
	}

	float RandomFloat(float aMin, float aMax)
	{
		static std::random_device rndDevice;
		static std::mt19937 mt(rndDevice());

		std::uniform_real_distribution<float> rndDist(aMin, aMax);

		return rndDist(mt);
	}
}