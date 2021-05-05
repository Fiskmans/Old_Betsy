#include <pch.h>
#include "Random.h"
#include <random>

std::mt19937& random()
{
	static std::random_device seed;
#ifdef _DEBUG
	static std::mt19937 rng(1337);
#else
	static std::mt19937 rng(seed());
#endif // _DEBUG
	return rng;
}

float Tools::RandomNormalized()
{
	static std::uniform_real_distribution<float> dist(0.0f, 1.f);
	return dist(random());
}

size_t Tools::RandomRange(size_t aMin, size_t aMax)
{
	std::uniform_int_distribution dist(aMin, aMax);
	return dist(random());
}

int Tools::RandomRange(int aMin, int aMax)
{
	std::uniform_int_distribution dist(aMin, aMax);
	return dist(random());
}

float Tools::RandomRange(float aMin, float aMax)
{
	std::uniform_real_distribution<float> dist(aMin,aMax);
	return dist(random());
}

V2F Tools::RandomRange(const V2F& aMin, const V2F& aMax)
{
	return V2F(RandomRange(aMin.x, aMax.x), RandomRange(aMin.y, aMax.y));
}

V3F Tools::RandomRange(const V3F& aMin, const V3F& aMax)
{
	return V3F(RandomRange(aMin.x, aMax.x), RandomRange(aMin.y, aMax.y), RandomRange(aMin.z, aMax.z));
}

V4F Tools::RandomRange(const V4F& aMin, const V4F& aMax)
{
	return V4F(RandomRange(aMin.x,aMax.x), RandomRange(aMin.y, aMax.y), RandomRange(aMin.z, aMax.z), RandomRange(aMin.w, aMax.w));
}

V3F Tools::RandomDirection()
{
	//https://www.bogotobogo.com/Algorithms/uniform_distribution_sphere.php
	float theta = 2.f * 3.141592f * RandomNormalized();
	float phi = acos(2.f * RandomNormalized() - 1.0f);
	float x = cos(theta) * sin(phi);
	float y = sin(theta) * sin(phi);
	float z = cos(phi);
	return V3F(x,y,z);
}


