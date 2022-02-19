#pragma once

namespace math
{
	namespace marching_cubes_lut
	{
		struct Tri
		{
			int myIndex1;
			int myIndex2;
			int myIndex3;
		};

		extern const unsigned char globalTriCountLUT[256];
		extern const Tri globalTriLUT[256][5];
	}

	template<std::random_access_iterator Iterator>
	requires(std::is_convertible_v<std::iter_reference_t<Iterator>, float>)
	std::vector<marching_cubes::Tri> MarchingCubes(const Iterator aData, const CommonUtilities::Vector3<size_t> aStepSize, const CommonUtilities::Vector3<size_t> aSize, float aOffset)
	{

		std::vector<marching_cubes::Tri> out;

		out.reserve(aSize.Sum());

		const V3F cellSize = V3F(1.f / (aSize.x - 1), 1.f / (aSize.y - 1), 1.f / (aSize.z - 1));

		for (size_t z = 0; z < aSize.z - 1; z++)
		{
			for (size_t y = 0; y < aSize.y - 1; y++)
			{
				for (size_t x = 0; x < aSize.x - 1; x++)
				{
					const CommonUtilities::Vector3<size_t> indexBase{ x, y, z };

					const float weigths[8] = {
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(0, 0, 0)) * aStepSize).Sum()]) + aOffset + 1.f,
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(1, 0, 0)) * aStepSize).Sum()]) + aOffset + 1.f,
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(0, 1, 0)) * aStepSize).Sum()]) + aOffset + 1.f,
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(1, 1, 0)) * aStepSize).Sum()]) + aOffset + 1.f,
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(0, 0, 1)) * aStepSize).Sum()]) + aOffset + 1.f,
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(1, 0, 1)) * aStepSize).Sum()]) + aOffset + 1.f,
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(0, 1, 1)) * aStepSize).Sum()]) + aOffset + 1.f,
						static_cast<float>(aData[((indexBase + CommonUtilities::Vector3<size_t>(1, 1, 1)) * aStepSize).Sum()]) + aOffset + 1.f
					};
					uint8_t index = 0b00000000;
					for (size_t i = 0; i < 8; i++)
					{
						index |= ((weigths[i] > 1.f) << i);
					}

					if (index == 0b00000000 || index == 0b11111111) { continue; } // surface does not go through this cell


					// vertex indexes
					//               6________________7        
					//              /|               /|         
					//             / |              / |         
					//            /  |             /  |         
					//          4/_______________5/   |         
					//          |    |           |    |         
					//          |    |           |    |         
					//          |    |           |    |         
					//          |    2___________|____3         
					//          |   /            |   /          
					//          z  /             |  /           
					//          | y              | /            
					//          0/__x____________1/             
					
					// edge indexes
					//                ________6_______         
					//              /|               /|         
					//             5 |              7 |         
					//            /  |             /  |         
					//           /_______4________/   11        
					//          |    10          |    |         
					//          |    |           |    |         
					//          8    |           9    |         
					//          |    |______2____|____|         
					//          |   /            |   /          
					//          y  1             |  3           
					//          | z              | /            
					//          |/__x____0_______|/             


					const V3F basePoint = V3F(static_cast<float>(x), static_cast<float>(z), static_cast<float>(y)) * cellSize;

					const V3F points[12] =
					{
						basePoint + V3F(cellSize.x * INVERSELERP(weigths[0], weigths[1], 1.f),	0,														0),													   	
						basePoint + V3F(cellSize.x,												0,														cellSize.z * INVERSELERP(weigths[1], weigths[3], 1.f)),
						basePoint + V3F(cellSize.x * INVERSELERP(weigths[2], weigths[3], 1.f),	0,														cellSize.z),										   
						basePoint + V3F(0,														0,														cellSize.z * INVERSELERP(weigths[0], weigths[2], 1.f)),
						basePoint + V3F(cellSize.x * INVERSELERP(weigths[4], weigths[5], 1.f),	cellSize.y,												0),													   
						basePoint + V3F(cellSize.x,												cellSize.y,												cellSize.z * INVERSELERP(weigths[5], weigths[7], 1.f)),
						basePoint + V3F(cellSize.x * INVERSELERP(weigths[6], weigths[7], 1.f),	cellSize.y,												cellSize.z),										   
						basePoint + V3F(0,														cellSize.y,												cellSize.z * INVERSELERP(weigths[4], weigths[6], 1.f)),
						basePoint + V3F(0,														cellSize.y * INVERSELERP(weigths[0], weigths[4], 1.f),	0),													   
						basePoint + V3F(cellSize.x,												cellSize.y * INVERSELERP(weigths[1], weigths[5], 1.f),	0),													   
						basePoint + V3F(0,														cellSize.y * INVERSELERP(weigths[2], weigths[6], 1.f),	cellSize.z),										   
						basePoint + V3F(cellSize.x,												cellSize.y * INVERSELERP(weigths[3], weigths[7], 1.f),	cellSize.z),										   
					};

					for (int i = 0; i < marching_cubes_lut::globalTriCountLUT[index]; i++)
					{
						const marching_cubes_lut::Tri& tri = marching_cubes_lut::globalTriLUT[index][i];

						if (   std::isinf(points[tri.myIndex1].x) || std::isnan(points[tri.myIndex1].x) || std::isinf(points[tri.myIndex1].y) || std::isnan(points[tri.myIndex1].y) || std::isinf(points[tri.myIndex1].z) || std::isnan(points[tri.myIndex1].z)
							|| std::isinf(points[tri.myIndex2].x) || std::isnan(points[tri.myIndex2].x) || std::isinf(points[tri.myIndex2].y) || std::isnan(points[tri.myIndex2].y) || std::isinf(points[tri.myIndex2].z) || std::isnan(points[tri.myIndex2].z)
							|| std::isinf(points[tri.myIndex3].x) || std::isnan(points[tri.myIndex3].x) || std::isinf(points[tri.myIndex3].y) || std::isnan(points[tri.myIndex3].y) || std::isinf(points[tri.myIndex3].z) || std::isnan(points[tri.myIndex3].z))
						{
							using namespace std::string_literals;
							SYSERROR("marching cubes nan/infed out", 
								"points: [" 
								+ std::to_string(points[0].x) + ", "
								+ std::to_string(points[1].x) + ", "
								+ std::to_string(points[2].x) + ", "
								+ std::to_string(points[3].x) + ", "
								+ std::to_string(points[4].x) + ", "
								+ std::to_string(points[5].x) + ", "
								+ std::to_string(points[6].x) + ", "
								+ std::to_string(points[7].x) + ", "
								+ std::to_string(points[8].x) + ", "
								+ std::to_string(points[9].x) + ", "
								+ std::to_string(points[10].x) + ", "
								+ std::to_string(points[11].x) + "]",
								"index: " + std::to_string(index),
								"tri " + std::to_string(i+1) + "/" + std::to_string(marching_cubes_lut::globalTriCountLUT[index]),
								"A: {" + std::to_string(points[tri.myIndex1].x) + ", " + std::to_string(points[tri.myIndex1].y) + ", " + std::to_string(points[tri.myIndex1].z) + " }",
								"B: {" + std::to_string(points[tri.myIndex2].x) + ", " + std::to_string(points[tri.myIndex2].y) + ", " + std::to_string(points[tri.myIndex2].z) + " }",
								"C: {" + std::to_string(points[tri.myIndex3].x) + ", " + std::to_string(points[tri.myIndex3].y) + ", " + std::to_string(points[tri.myIndex3].z) + " }"
							)
							continue;
						}

						out.push_back(marching_cubes::Tri{ points[tri.myIndex1], points[tri.myIndex2], points[tri.myIndex3] });
					}
				}
			}
		}

		return out;
	}
}