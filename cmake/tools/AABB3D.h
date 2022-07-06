#ifndef TOOLS_AABB3D_H
#define TOOLS_AABB3D_H

#include "tools/MathVector.h"

namespace tools
{
	template<typename T>
	class AABB3D
	{
	public:

		AABB3D() = default;
		AABB3D(const AABB3D<T>& aAABB3D) = default;
		AABB3D(const tools::MathVector<T, 3>& aMin, const tools::MathVector<T, 3>& aMax);

		bool IsInside(const tools::MathVector<T, 3>& aPosition) const;

		tools::MathVector<T, 3>& Min();
		const tools::MathVector<T, 3>& Min() const;
		tools::MathVector<T, 3>& Max();
		const tools::MathVector<T, 3>& Max() const;

	private:
		tools::MathVector<T, 3> myMin;
		tools::MathVector<T, 3> myMax;
	};

	template<typename T>
	inline AABB3D<T>::AABB3D(const tools::MathVector<T, 3>& aMin, const tools::MathVector<T, 3>& aMax) :
		myMin(aMin),
		myMax(aMax)
	{
	}

	template<typename T>
	inline tools::MathVector<T, 3>& AABB3D<T>::Min()
	{
		return myMin;
	}


	template<typename T>
	inline const tools::MathVector<T, 3>& AABB3D<T>::Min() const
	{
		return myMin;
	}

	template<typename T>
	inline tools::MathVector<T, 3>& AABB3D<T>::Max()
	{
		return myMax;
	}

	template<typename T>
	inline const tools::MathVector<T, 3>& AABB3D<T>::Max() const
	{
		return myMax;
	}

	template<typename T>
	inline bool AABB3D<T>::IsInside(const tools::MathVector<T, 3>& aPosition) const
	{
		return 
			aPosition.x >= myMin.x && aPosition.x <= myMax.x &&
			aPosition.y >= myMin.y && aPosition.y <= myMax.y &&
			aPosition.z >= myMin.z && aPosition.z <= myMax.z;
	}
}

#endif