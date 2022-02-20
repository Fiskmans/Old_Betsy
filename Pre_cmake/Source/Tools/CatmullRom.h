#pragma once
#include <vector>
#include <array>

namespace Math
{
	template<class T>
	T GetCatmullRomPosition(float t, T p0, T p1, T p2, T p3)
	{
		//The coefficients of the cubic polynomial (except the 0.5f * which I added later for performance)
		const T a = 2.f * p1;
		const T b = p2 - p0;
		const T c = 2.f * p0 - 5.f * p1 + 4.f * p2 - p3;
		const T d = -p0 + 3.f * p1 - 3.f * p2 + p3;

		//The cubic polynomial: a + b * t + c * t^2 + d * t^3
		const T pos = 0.5f * (a + (b * t) + (c * t * t) + (d * t * t * t));

		return pos;
	}

	template<class T>
	std::vector<T> CatmullRomFragmentize(std::vector<T> aListOfPoints, size_t aPointsPerControlPoint)
	{
		std::vector<T> out;
		if (aListOfPoints.size() < 3)
		{
			return out;
		}

		std::array<T, 4> currentPoints;
		currentPoints[0] = aListOfPoints[0];
		currentPoints[1] = aListOfPoints[0];
		currentPoints[2] = aListOfPoints[0];
		currentPoints[3] = aListOfPoints[1];
		for (size_t i = 2; i <= aListOfPoints.size(); i++)
		{
			currentPoints[0] = currentPoints[1];
			currentPoints[1] = currentPoints[2];
			currentPoints[2] = currentPoints[3];
			currentPoints[3] = (i < aListOfPoints.size()) ? aListOfPoints[i] : aListOfPoints.back();

			for (float f = 0; f < 1.0f; f += 1.f/aPointsPerControlPoint)
			{
				out.push_back(GetCatmullRomPosition(f, currentPoints[0], currentPoints[1], currentPoints[2], currentPoints[3]));
			}
		}
		return out;

	}
}
