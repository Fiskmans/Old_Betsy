#pragma once
#include <array>


namespace Math
{
	template<class T,size_t Count>
	T BezierInterpolation(std::array<T, Count> aPoints, float aT);

	template<class T,size_t Count>
	T BezierFourPointInterpolation(std::array<T, Count> aPoints, float aT);





	template<class T,size_t Count>
	T BezierInterpolation(std::array<T, Count> aPoints, float aT)
	{
		float part = 1.f - aT;
		float invers =  aT;

		std::array<T, Count> points = aPoints;

		for (size_t order = Count-1; order >= 1; --order)
		{
			for (size_t i = 0; i < order; i++)
			{
				points[i] = points[i] * part + points[i + 1] * invers;
			}
		}
		return points[0];
	}
	template<class T, size_t Count>
	T BezierFourPointInterpolation(std::array<T, Count> aPoints, float aT)
	{
		//static_assert(((Count - 1) % 3) == 0); // trying to interpolate invalid point collection
		size_t segmentCount = ((Count - 1) / 3);
		size_t segment = segmentCount * aT;
		
		std::array<T, 4> points = { aPoints[segment * 3],aPoints[segment * 3 + 1],aPoints[segment * 3 + 2],aPoints[segment * 3 + 3] };
		float partOfSegment = fmod(aT, 1.f / static_cast<float>(segmentCount)) * segmentCount;

		return BezierInterpolation(points,partOfSegment);
	}
}