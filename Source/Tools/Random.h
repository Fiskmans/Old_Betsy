#pragma once
#include <Vector.hpp>
namespace Tools
{
	float RandomNormalized();
	size_t RandomRange(size_t aMin, size_t aMax);
	int RandomRange(int aMin, int aMax);
	float RandomRange(float aMin, float aMax);
	V2F RandomRange(const V2F& aMin, const V2F& aMax);
	V3F RandomRange(const V3F& aMin, const V3F& aMax);
	V4F RandomRange(const V4F& aMin, const V4F& aMax);

	V3F RandomDirection();
}
