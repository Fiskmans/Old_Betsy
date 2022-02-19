#include "pch.h"
#include "Fourier.h"

namespace Math
{
	FourierTransformValue DiscreteFourierTransform_Internal(const std::vector<CompF>& aValues, int aFrequencyToCheck)
	{
		CompF val;
		for (size_t i = 0; i < aValues.size(); i++)
		{
			val += aValues[i] * CompF::EPow(static_cast<float>(TAU * -aFrequencyToCheck * i / (aValues.size() - 1)));
		}

		return { aFrequencyToCheck, val * (1.f / static_cast<float>(aValues.size())) };
	}

	std::vector<FourierTransformValue> DiscreteFourierTransform(const std::vector<CompF>& aValues, size_t aFrequenciesToCheck)
	{
		std::vector<FourierTransformValue> out;

		out.push_back(DiscreteFourierTransform_Internal(aValues, 0));

		for (int i = 1; i < aFrequenciesToCheck+1; i++)
		{
			out.push_back(DiscreteFourierTransform_Internal(aValues, i));
			out.push_back(DiscreteFourierTransform_Internal(aValues, -i));
		}

		return out;
	}

	std::vector<CompF> DescreteFourierInterpolateSteps(const std::vector<FourierTransformValue>& aSeries, float aValue)
	{
		std::vector<CompF> out;
		CompF at(0.f, ImagF(0.f));
		out.push_back(at);
		for (auto& i : aSeries)
		{
			at += i.myC * CompF::EPow(static_cast<float>(i.myFrequency * TAU * aValue));
			out.push_back(at);
		}
		return out;
	}

	CompF DescreteFourierInterpolate(const std::vector<FourierTransformValue>& aSeries, float aValue)
	{
		CompF at(0.f, ImagF(0.f));
		for (auto& i : aSeries)
		{
			at += i.myC * CompF::EPow(static_cast<float>(i.myFrequency * TAU * aValue));
		}
		return at;
	}
}