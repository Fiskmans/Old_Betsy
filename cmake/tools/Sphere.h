#ifndef TOOLS_SPHERE_H
#define TOOLS_SPHERE_H

#include "tools/MathVector.h"

namespace tools
{
	template<typename T>
	class Sphere
	{
	public:
		Sphere() = default;
		Sphere(const tools::MathVector<T, 3>& aCenter, T aRadius);

		bool Intersects(const Sphere& aSphere) const;

		tools::MathVector<T, 3> myPosition;
		T myRadius;
	};

	template<typename T>
	inline Sphere<T>::Sphere(const tools::MathVector<T, 3>& aCenter, T aRadius) :
		myPosition(aCenter),
		myRadius(aRadius)
	{
	}

	template<typename T>
	inline bool Sphere<T>::Intersects(const Sphere& aSphere) const
	{
		const MathVector<T, 3> delta = myPosition - aSphere.myPosition;
		const T intersectionDistanceSq = tools::Square(myRadius + aSphere.myRadius);

		return delta.LengthSqr() < intersectionDistanceSq;
	}
}

#endif