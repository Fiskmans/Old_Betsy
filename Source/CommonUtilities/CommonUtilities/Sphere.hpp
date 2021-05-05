#pragma once
#include "Vector3.hpp"

namespace CommonUtilities
{
	template<typename T>
	class Sphere
	{
	public:
		// Default constructor: there is no sphere, the radius is zero and the position is
		// the zero vector.
		Sphere();
		
		// Copy constructor.
		Sphere(const Sphere<T>& aSphere);

		// Constructor that takes the center position and radius of the sphere.
		Sphere(const Vector3<T>& aCenter, T aRadius);

		// Init the sphere with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius);

		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInside(const Vector3<T>& aPosition) const;

		T Radius() const;
		const Vector3<T>& Position() const;
	public:
		Vector3<T> myPosition;
		T myRadius;
	};

	template<typename T>
	inline Sphere<T>::Sphere() :
		myRadius(0)
	{
	}
	
	template<typename T>
	inline Sphere<T>::Sphere(const Sphere<T>& aSphere) :
		myRadius(aSphere.myRadius),
		myPosition(aSphere.myPosition)
	{
	}

	template<typename T>
	inline Sphere<T>::Sphere(const Vector3<T>& aCenter, T aRadius) :
		myPosition(aCenter),
		myRadius(aRadius)
	{
	}

	template<typename T>
	inline void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
	{
		myPosition = aCenter;
		myRadius = aRadius;
	}
	
	template<typename T>
	inline T Sphere<T>::Radius() const
	{
		return myRadius;
	}

	template<typename T>
	inline const Vector3<T>& Sphere<T>::Position() const
	{
		return myPosition;
	}

	template<typename T>
	inline bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
	{
		const T dist = (aPosition.x - myPosition.x) + (aPosition.y - myPosition.y) + (aPosition.z - myPosition.z);
		return dist * dist <= myRadius * myRadius;
	}
}