#pragma once
#include "Vector3.hpp"
#include "Plane.hpp"

namespace CommonUtilities
{
	template<typename T>
	class Ray
	{
	public:
		Ray();
		Ray(const Ray<T>& aRay);
		Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);
		void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

		bool FindIntersection(const Plane<T>& aPlane, Vector3<T>& aOutPoint = ourOutDefaultVector, T& aOutDistance = ourOutDefaultScalar) const;
		bool FindIntersection(const Plane<T>& aPlane, T& aOutDistance) const;

		const Vector3<T>& Position() const;
		const Vector3<T>& Direction() const;

		Vector3<T> PointAtDistance(const T& aDistance) const;

	private:
		static thread_local Vector3<T> ourOutDefaultVector;
		static thread_local T ourOutDefaultScalar;

		Vector3<T> myPosition;
		Vector3<T> myDirection;
	};

	template<typename T>
	thread_local Vector3<T> Ray<T>::ourOutDefaultVector;

	template<typename T>
	thread_local T Ray<T>::ourOutDefaultScalar;

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
	inline Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection) :
		myPosition(aOrigin),
		myDirection(aDirection)
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
	inline bool Ray<T>::FindIntersection(const Plane<T>& aPlane, T& aOutDistance) const
	{
		return FindIntersection(aPlane,ourOutDefaultVector,aOutDistance);
	}

	template<typename T>
	inline bool Ray<T>::FindIntersection(const Plane<T>& aPlane, Vector3<T>& aOutPoint, T & aOutDistance) const
	{
		Vector3<T> toPlane = aPlane.Point() - myPosition;
		toPlane = toPlane.Dot(aPlane.Normal()) * aPlane.Normal();

		aOutDistance = toPlane.Dot(myDirection);

		if (aOutDistance > 0)
		{
			aOutPoint = myPosition + aOutDistance * myDirection;
			return true;
		}
		return false;
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
	template<typename T>
	inline Vector3<T> Ray<T>::PointAtDistance(const T& aDistance) const
	{
		return myPosition + myDirection * aDistance;
	}
}

typedef CommonUtilities::Ray<float> FRay;
typedef CommonUtilities::Ray<double> DRay;