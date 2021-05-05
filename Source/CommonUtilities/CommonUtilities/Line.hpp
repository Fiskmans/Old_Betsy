#pragma once
#include "Vector.hpp"
#include <cmath>
#include <cassert>

#define EPSILON 0.0001f
#define EPSILON_D 0.0001

namespace CommonUtilities
{
	template<typename T>
	class Line
	{
	public:
		Line();
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		~Line() = default;

		Line& operator=(const Line& aOther) = default;
		bool operator==(const Line& aOther) const;

		void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);
		bool Inside(const Vector2<T>& aPosition) const;

		bool PointIsOnLine(Vector2<T>& aPoint) const;
		Vector2<T> Normal() const;
		const Vector2<T>& Direction() const;
		const Vector2<T>& Point1() const;
		const Vector2<T>& Point2() const;
		const Vector2<T>& GetIntersection(const Line& aOther) const;
		const Vector2<T>& GetIntersection(const Vector2<T>& aPoint1, const Vector2<T>& aPoint2) const;
	private:
		Vector2<T> myPoint;
		Vector2<T> myDirection;
	};

	template<typename T>
	inline Line<T>::Line() :
		myPoint(0, 0),
		myDirection(0, 0)
	{
	}

	template<typename T>
	inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1) :
		myPoint(aPoint0),
		myDirection(aPoint1 - aPoint0)
	{
	}

	template<typename T>
	inline bool Line<T>::operator==(const Line& aOther) const
	{
		return myPoint == aOther.myPoint && myDirection == aOther.myDirection;
	}

	template<typename T>
	inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		myDirection = aPoint1 - aPoint0;
	}

	template<typename T>
	inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
	{
		myPoint = aPoint;
		myDirection = aDirection;
	}

	template<typename T>
	inline bool Line<T>::Inside(const Vector2<T>& aPosition) const
	{
		return Vector2<T>(-myDirection.y, myDirection.x).Dot(aPosition - myPoint) <= 0;
	}

	template<typename T>
	inline Vector2<T> Line<T>::Normal() const
	{
		return Vector2<T>(-myDirection.y, myDirection.x);
	}
	template<>
	inline Vector2<float> Line<float>::Normal() const
	{
		Vector2<float> vector(-myDirection.y, myDirection.x);
		vector.Normalize();
		return vector;
	}
	template<>
	inline Vector2<double> Line<double>::Normal() const
	{
		Vector2<double> vector(-myDirection.y, myDirection.x);
		vector.Normalize();
		return vector;
	}

	template<typename T>
	inline const Vector2<T>& Line<T>::Point1() const
	{
		return myPoint;
	}

	template<typename T>
	inline const Vector2<T>& Line<T>::Point2() const
	{
		return myPoint + myDirection;
	}

	template<typename T>
	inline bool Line<T>::PointIsOnLine(Vector2<T>& aPoint) const
	{
		return Vector2<T>(-myDirection.y, myDirection.x).Dot(aPoint - myPoint) == 0;
	}
	template<>
	inline bool Line<float>::PointIsOnLine(Vector2<float>& aPoint) const
	{
		float result = std::abs(Vector2<float>(-myDirection.y, myDirection.x).Dot(aPoint - myPoint));
		return result <= EPSILON && result >= -EPSILON;
	}
	template<>
	inline bool Line<double>::PointIsOnLine(Vector2<double>& aPoint) const
	{
		float result = std::abs(Vector2<double>(-myDirection.y, myDirection.x).Dot(aPoint - myPoint));
		return result <= EPSILON_D && result >= -EPSILON_D;
	}

	template<typename T>
	inline const Vector2<T>& Line<T>::Direction() const
	{
		return myDirection;
	}

	template<typename T>
	const Vector2<T>& Line<T>::GetIntersection(const Line& aOther) const
	{
		assert("Lines are parallell!" && std::fabs(myDirection.x) != std::fabs(aOther.myDirection.x) || std::fabs(myDirection.y) != std::fabs(aOther.myDirection.y));

		if (myDirection.x == 0)
		{
			T k2 = aOther.myDirection.y / aOther.myDirection.x;
			T m2 = aOther.myPoint.y - k2 * aOther.myPoint.x;

			T y = myPoint.x * k2 + m2;

			return Vector2<T>(myPoint.x, y);
		}
		if (aOther.myDirection.x == 0)
		{
			T k1 = myDirection.y / myDirection.x;
			T m1 = myPoint.y - k1 * myPoint.x;

			T y = aOther.myPoint.x * k1 + m1;

			return Vector2<T>(aOther.myPoint.x, y);
		}

		T k1 = myDirection.y / myDirection.x;
		T m1 = myPoint.y - k1 * myPoint.x;
		T k2 = aOther.myDirection.y / aOther.myDirection.x;
		T m2 = aOther.myPoint.y - k2 * aOther.myPoint.x;

		T x = (m2 - m1) / (k1 - k2);
		T y = x * k1 + m1;

		return Vector2<T>(x, y);

	}

	template<typename T>
	const Vector2<T>& Line<T>::GetIntersection(const Vector2<T>& aPoint1, const Vector2<T>& aPoint2) const
	{
		return GetIntersection(Line<T>(aPoint1, aPoint2));
	}
}