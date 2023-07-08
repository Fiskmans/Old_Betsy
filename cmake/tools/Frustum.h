#ifndef TOOLS_FRUSTUM_H
#define TOOLS_FRUSTUM_H

#include "tools/Plane.h"
#include "tools/Sphere.h"

#include <vector>

namespace tools
{
	template<typename T>
	struct Frustum
	{
		Frustum() = default;
		Frustum(Plane<T> aFront, Plane<T> aBack, Plane<T> aTop, Plane<T> aBottom, Plane<T> aLeft, Plane<T> aRight);

		bool Inside(const tools::MathVector<T, 3>& aPosition) const;
		bool Intersects(const tools::Sphere<T>& aSphere) const;

		Plane<T> myFront;
		Plane<T> myBack;
		Plane<T> myTop;
		Plane<T> myBottom;
		Plane<T> myLeft;
		Plane<T> myRight;
	};

	template<typename T>
	inline Frustum<T>::Frustum(Plane<T> aFront, Plane<T> aBack, Plane<T> aTop, Plane<T> aBottom, Plane<T> aLeft, Plane<T> aRight)
		: myFront(aFront)
		, myBack(aBack)
		, myTop(aTop)
		, myBottom(aBottom)
		, myLeft(aLeft)
		, myRight(aRight)
	{
	}

	template<typename T>
	inline bool Frustum<T>::Inside(const tools::MathVector<T, 3>& aPosition) const
	{
		bool result = true;

		result &= myFront.Inside(aPosition);
		result &= myBack.Inside(aPosition);
		result &= myTop.Inside(aPosition);
		result &= myBottom.Inside(aPosition);
		result &= myLeft.Inside(aPosition);
		result &= myRight.Inside(aPosition);

		return result;
	}

	template<typename T>
	inline bool Frustum<T>::Intersects(const tools::Sphere<T>& aSphere) const
	{
		bool result = true;

		result &= myFront.Intersects(aSphere);
		result &= myBack.Intersects(aSphere);
		result &= myTop.Intersects(aSphere);
		result &= myBottom.Intersects(aSphere);
		result &= myLeft.Intersects(aSphere);
		result &= myRight.Intersects(aSphere);

		return result;
	}
}

#endif