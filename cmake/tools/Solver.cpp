#include "tools/Solver.h"

#include <numeric>
#include "Solver.h"

namespace tools
{
	float FindMinimum(std::function<float(float)> aFunction, float aBegin, float aEnd, size_t aInitialSegments, float aResultResolution)
	{
		float range[2] = { aBegin, aEnd };
		float lowest = std::numeric_limits<float>::max();

		const float stepSize = (aEnd - aBegin) / aInitialSegments;
		for (size_t i = 0; i <= aInitialSegments - 1; i++)
		{
			float at = aBegin + i * stepSize;
			float value = aFunction(at);

			if (value < lowest)
			{
				range[0] = at - stepSize;
				range[1] = at + stepSize;
				
				lowest = value;
			}
		}
		range[0] = std::max(range[0], aBegin);
		range[1] = std::min(range[1], aEnd);

		return FindLocalMinimum(aFunction, range[0], range[1], aResultResolution);
	}

	float FindLocalMinimum(std::function<float(float)> aFunction, float aBegin, float aEnd, float aResolution)
	{
		float begin = aBegin;
		float end = aEnd;

		float beginValue = aFunction(begin);
		float endValue = aFunction(end);

		while (end - begin > aResolution)
		{
			float next = (end + begin) / 2.f;
			float nextValue = aFunction(next);

			if (beginValue < endValue)
			{
				end = next;
				endValue = nextValue;
			}
			else
			{
				begin = next;
				beginValue = nextValue;
			}
		}

		return (end + begin) / 2.f;
	}
}