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
		Sphere(const Sphere<T>& aSphere);
		Sphere(const tools::MathVector<T, 3>& aCenter, T aRadius);

		bool IsInside(const tools::MathVector<T, 3>& aPosition) const;

		T Radius() const;
		const tools::MathVector<T, 3>& Position() const;
	public:
		tools::MathVector<T, 3> myPosition;
		T myRadius;
	};

	template<typename T>
	inline Sphere<T>::Sphere(const Sphere<T>& aSphere) :
		myRadius(aSphere.myRadius),
		myPosition(aSphere.myPosition)
	{
	}

	template<typename T>
	inline Sphere<T>::Sphere(const tools::MathVector<T, 3>& aCenter, T aRadius) :
		myPosition(aCenter),
		myRadius(aRadius)
	{
	}

	template<typename T>
	inline T Sphere<T>::Radius() const
	{
		return myRadius;
	}

	template<typename T>
	inline const tools::MathVector<T, 3>& Sphere<T>::Position() const
	{
		return myPosition;
	}

	template<typename T>
	inline bool Sphere<T>::IsInside(const tools::MathVector<T, 3>& aPosition) const
	{
		const T dist = (aPosition.x - myPosition.x) + (aPosition.y - myPosition.y) + (aPosition.z - myPosition.z);
		return dist * dist <= myRadius * myRadius;
	}
}

#endif