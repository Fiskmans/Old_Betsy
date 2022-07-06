#pragma once
#include "tools/MathVector.h"

namespace tools
{
	template<typename T>
	class Ray
	{
	public:
		Ray();
		Ray(const Ray<T>& aRay);
		Ray(const tools::MathVector<T, 3>& aOrigin, const tools::MathVector<T, 3>& aDirection);
		void InitWith2Points(const tools::MathVector<T, 3>& aOrigin, const tools::MathVector<T, 3>& aPoint);
		void InitWithOriginAndDirection(const tools::MathVector<T, 3>& aOrigin, const tools::MathVector<T, 3>& aDirection);

		bool FindIntersection(const Plane<T>& aPlane, tools::MathVector<T, 3>& aOutPoint = ourOutDefaultVector, T& aOutDistance = ourOutDefaultScalar) const;
		bool FindIntersection(const Plane<T>& aPlane, T& aOutDistance) const;

		const tools::MathVector<T, 3>& Position() const;
		const tools::MathVector<T, 3>& Direction() const;

		tools::MathVector<T, 3> PointAtDistance(const T& aDistance) const;

	private:
		static thread_local tools::MathVector<T, 3> ourOutDefaultVector;
		static thread_local T ourOutDefaultScalar;

		tools::MathVector<T, 3> myPosition;
		tools::MathVector<T, 3> myDirection;
	};

	using FRay = Ray<float>;

	template<typename T>
	thread_local tools::MathVector<T, 3> Ray<T>::ourOutDefaultVector;

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
	inline Ray<T>::Ray(const tools::MathVector<T, 3>& aOrigin, const tools::MathVector<T, 3>& aDirection) :
		myPosition(aOrigin),
		myDirection(aDirection)
	{
	}
	template<typename T>
	inline void Ray<T>::InitWith2Points(const tools::MathVector<T, 3>& aOrigin, const tools::MathVector<T, 3>& aPoint)
	{
		myPosition = aOrigin;
		myDirection = aPoint - aOrigin;
	}
	template<typename T>
	inline void Ray<T>::InitWithOriginAndDirection(const tools::MathVector<T, 3>& aOrigin, const tools::MathVector<T, 3>& aDirection)
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
	inline bool Ray<T>::FindIntersection(const Plane<T>& aPlane, tools::MathVector<T, 3>& aOutPoint, T & aOutDistance) const
	{
		tools::MathVector<T, 3> toPlane = aPlane.Point() - myPosition;
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
	inline const tools::MathVector<T, 3>& Ray<T>::Position() const
	{
		return myPosition;
	}

	template<typename T>
	inline const tools::MathVector<T, 3>& Ray<T>::Direction() const
	{
		return myDirection;
	}

	template<typename T>
	inline tools::MathVector<T, 3> Ray<T>::PointAtDistance(const T& aDistance) const
	{
		return myPosition + myDirection * aDistance;
	}
}