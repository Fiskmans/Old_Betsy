#pragma once
#include "Vector3.hpp"
#include "Plane.hpp"

namespace CommonUtilities
{
	template<typename T>
	class Ray
	{
	public:
		// Default constructor: there is no ray, the origin and direction are the
		// zero vector.
		Ray();
		// Copy constructor.
		Ray(const Ray<T>& aRay);
		// Constructor that takes two points that define the ray, the direction is
		// aPoint - aOrigin and the origin is aOrigin.
		Ray(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);
		// Init the ray with two points, the same as the constructor above.
		void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);
		// Init the ray with an origin and a direction.
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

		Vector3<T> FindIntersection(Plane<T> aPlane);

		const Vector3<T>& Position() const;
		const Vector3<T>& Direction() const;
	private:
		Vector3<T> myPosition;
		Vector3<T> myDirection;
	};

	template<typename T>
	inline Ray<T>::Ray()
	{
	}

	template<typename T>
	inline Ray<T>::Ray(const Ray<T>& aRay) :
		myPosition(aRay.myPosition),
		myDirection(aRay.myDirection)
	{
	}

	template<typename T>
	inline Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aPoint) :
		myPosition(aOrigin),
		myDirection(aPoint - aOrigin)
	{
	}
	template<typename T>
	inline void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myPosition = aOrigin;
		myDirection = aPoint - aOrigin;
	}
	template<typename T>
	inline void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myPosition = aOrigin;
		myDirection = aDirection;
	}
	template<typename T>
	inline const Vector3<T>& Ray<T>::Position() const
	{
		return myPosition;
	}
	template<typename T>
	inline const Vector3<T>& Ray<T>::Direction() const
	{
		return myDirection;
	}
}