#pragma once
#include <vector>
#include "Complex.h"

namespace Math
{
	struct FourierTransformValue
	{
		int myFrequency;
		CompF myC;
	};

	///
	// <aValues>The input values to do the transform on<aValues/>
	// <aFrequenciesToCheck>The wanted precision<aFrequenciesToCheck/>
	// <return>a Series of FourierTransformValue's with the size of (aFrequenciesToCheck*2 + 1) that as closely as possibly will regererate aValues when added together over the 0-1 space<return>
	std::vector<FourierTransformValue> DiscreteFourierTransform(const std::vector<CompF>& aValues, size_t aFrequenciesToCheck);

	std::vector<CompF> DescreteFourierInterpolateSteps(const std::vector<FourierTransformValue>& aSeries, float aValue);
	CompF DescreteFourierInterpolate(const std::vector<FourierTransformValue>& aSeries, float aValue);
}


