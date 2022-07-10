#ifndef TOOLS_SOLVER_H
#define TOOLS_SOLVER_H

#include <functional>

namespace tools
{
	float FindMinimum(std::function<float(float)> aFunction, float aBegin, float aEnd, size_t aInitialSegments, float aResultResolution);
	float FindLocalMinimum(std::function<float(float)> aFunction, float aBegin, float aEnd, float aResolution);
}

#endif