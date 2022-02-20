#pragma once

#include "CommonUtilities/Vector3.hpp"
#include <concepts>

namespace math {
	
	namespace marching_cubes
	{
		/// <summary>
		/// A directed Tri
		/// Normal = (A - B).Cross(A - C)
		/// </summary>
		struct Tri {
			V3F myA;
			V3F myB;
			V3F myC;
		};
	}	

	/// <summary>
	/// Employs a marching cubes algorithm to generate geometry based on the boundary where input data (+ offset) crosses over 0
	/// </summary>
	/// <param name="aData">A Random access iterator to values convertible to float at the base { 0, 0, 0 } of the geometry to generate</param>
	/// <param name="aStepSize">How much to offset aData by to reach the next element in the { x, y, z } directions</param>
	/// <param name="aSize">How many data points you want to generate geometry for</param>
	/// <param name="aOffset">How much to offset the threshold</param>
	/// <returns>A collection of Tris that estimates the 0 boundary of the input data. Tris will be scaled to always fit in a unit cube, the max element count is [(aSize.x - 1) * (aSize.y - 1) * (aSize.z - 1) * 5]</returns>
	template<std::random_access_iterator Iterator>
	requires(std::is_convertible_v<std::iter_reference_t<Iterator>, float>)
	std::vector<marching_cubes::Tri> MarchingCubes(const Iterator aData, const CommonUtilities::Vector3<size_t> aStepSize, const CommonUtilities::Vector3<size_t> aSize, float aOffset = 0);
}

#include "MarchingCubes_impl.h"
