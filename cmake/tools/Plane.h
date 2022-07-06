#ifndef TOOLS_PLANE_H
#define TOOLS_PLANE_H

#include "tools/MathVector.h"

#define EPSILON 0.0001f
#define EPSILON_D 0.0001

namespace tools
{
	template<typename T>
	class Plane
	{
	public:
		Plane();
		Plane(const tools::MathVector<T, 3>& aPoint0, const tools::MathVector<T, 3>& aPoint1, const tools::MathVector<T, 3>& aPoint2);
		Plane(const tools::MathVector<T, 3>& aPoint, const tools::MathVector<T, 3>& aNormal);
		~Plane() = default;

		bool Inside(const tools::MathVector<T, 3>& aPosition) const;

		const tools::MathVector<T, 3>& Point() const;
		const tools::MathVector<T, 3>& Normal() const;

	private:
		tools::MathVector<T, 3> myPoint;
		tools::MathVector<T, 3> myNormal;
	};

	template<typename T>
	inline Plane<T>::Plane() :
		myPoint (0, 0, 0),
		myNormal (0, 0, 0)
	{
	}

	template<typename T>
	inline Plane<T>::Plane(const tools::MathVector<T, 3>& aPoint0, const tools::MathVector<T, 3>& aPoint1, const tools::MathVector<T, 3>& aPoint2)
	{
		myPoint = aPoint0;
		myNormal = tools::MathVector<T, 3>(aPoint1 - aPoint0).Cross(tools::MathVector<T, 3>(aPoint2 - aPoint0)).GetNormalized();
	}

	template<typename T>
	inline Plane<T>::Plane(const tools::MathVector<T, 3>& aPoint, const tools::MathVector<T, 3>& aNormal)
	{
		myPoint = aPoint;
		myNormal = aNormal.GetNormalized();
	}

	template<typename T>
	inline bool Plane<T>::Inside(const tools::MathVector<T, 3>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= 0;
	}

	template<>
	inline bool Plane<float>::Inside(const tools::MathVector<float, 3>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= EPSILON;
	}

	template<>
	inline bool Plane<double>::Inside(const tools::MathVector<double, 3>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= EPSILON_D;
	}
	template<typename T>
	const tools::MathVector<T, 3>& Plane<T>::Point() const
	{
		return myPoint;
	}

	template<typename T>
	const tools::MathVector<T, 3>& Plane<T>::Normal() const
	{
		return myNormal;
	}
}

#endif