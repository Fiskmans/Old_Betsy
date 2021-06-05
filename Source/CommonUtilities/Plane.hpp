#pragma once
#include "Vector.hpp"

#define EPSILON 0.0001f
#define EPSILON_D 0.0001

namespace CommonUtilities
{
	template<typename T>
	class Plane
	{
	public:
		Plane();
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal);
		~Plane() = default;

		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);

		bool Inside(const Vector3<T>& aPosition) const;

		const Vector3<T>& Point() const;
		const Vector3<T>& Normal() const;

	private:
		Vector3<T> myPoint;
		Vector3<T> myNormal;
	};

	template<typename T>
	inline Plane<T>::Plane() :
		myPoint (0, 0, 0),
		myNormal (0, 0, 0)
	{
	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		InitWith3Points(aPoint0, aPoint1, aPoint2);
	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
	{
		InitWithPointAndNormal(aPoint0, aNormal);
	}

	template<typename T>
	inline void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPoint = aPoint0;
		myNormal = Vector3<T>(aPoint1 - aPoint0).Cross(Vector3<T>(aPoint2 - aPoint0)).GetNormalized();
	}

	template<typename T>
	inline void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myPoint = aPoint;
		myNormal = aNormal.GetNormalized();
	}
	template<typename T>
	inline bool Plane<T>::Inside(const Vector3<T>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= 0;
	}

	template<>
	inline bool Plane<float>::Inside(const Vector3<float>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= EPSILON;
	}

	template<>
	inline bool Plane<double>::Inside(const Vector3<double>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= EPSILON_D;
	}
	template<typename T>
	const Vector3<T>& Plane<T>::Point() const
	{
		return myPoint;
	}

	template<typename T>
	const Vector3<T>& Plane<T>::Normal() const
	{
		return myNormal;
	}
}