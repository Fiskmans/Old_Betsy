#ifndef TOOLS_PLANE_VOLUME_H
#define TOOLS_PLANE_VOLUME_H

#include "tools/Plane.h"

#include <vector>

namespace tools
{
	template<typename T>
	class Sphere;

	template<typename T>
	class PlaneVolume
	{
		friend bool IntersectionSpherePlaneVolume(const Sphere<T>& aSphere, const PlaneVolume<T>& aPlaneVolume);

	public:
		PlaneVolume() = default;
		PlaneVolume(const std::vector<Plane<T>>& aPlaneList);

		void AddPlane(const Plane<T>& aPlane);

		bool Inside(const tools::MathVector<T, 3>& aPosition);

		int Size() const;

		void Clear();

		const std::vector<Plane<T>>& Planes() const { return myData; }
	private:
		std::vector<Plane<T>> myData;
	};

	template<typename T>
	using Frustum = PlaneVolume<T>;

	template<typename T>
	inline PlaneVolume<T>::PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
	{
		myData = aPlaneList;
	}

	template<typename T>
	inline void PlaneVolume<T>::AddPlane(const Plane<T>& aPlane)
	{
		myData.push_back(aPlane);
	}

	template<typename T>
	inline bool PlaneVolume<T>::Inside(const tools::MathVector<T, 3>& aPosition)
	{
		for (auto& iterator : myData)
		{
			if (!iterator.Inside(aPosition))
			{
				return false;
			}
		}

		return true;
	}
	template<typename T>
	inline int PlaneVolume<T>::Size() const
	{
		return static_cast<int>(myData.size());
	}

	template<typename T>
	inline void PlaneVolume<T>::Clear()
	{
		myData.clear();
	}
}

#endif