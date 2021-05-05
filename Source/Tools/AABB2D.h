#pragma once
#include "Vector2.hpp"

namespace CommonUtilities
{
	template<typename T>
	class AABB2D
	{
	public:
		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB2D();

		// Copy constructor.
		AABB2D(const AABB2D<T>& aAABB2D);

		// Constructor taking the positions of the minimum and maximum corners.
		AABB2D(const Vector2<T>& aMin, const Vector2<T>& aMax);

		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector2<T>& aMin, const Vector2<T>& aMax);

		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector2<T>& aPosition) const;
		Vector2<T>& Min();
		const Vector2<T>& Min() const;
		Vector2<T>& Max();
		const Vector2<T>& Max() const;

	private:
		Vector2<T> myMin;
		Vector2<T> myMax;
	};

	template<typename T>
	inline AABB2D<T>::AABB2D()
	{
	}

	template<typename T>
	inline AABB2D<T>::AABB2D(const AABB2D<T>& aAABB2D) :
		myMin(aAABB2D.myMin),
		myMax(aAABB2D.myMax)
	{
	}

	template<typename T>
	inline AABB2D<T>::AABB2D(const Vector2<T>& aMin, const Vector2<T>& aMax) :
		myMin(aMin),
		myMax(aMax)
	{
	}

	template<typename T>
	inline void AABB2D<T>::InitWithMinAndMax(const Vector2<T>& aMin, const Vector2<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}

	template<typename T>
	inline Vector2<T>& AABB2D<T>::Min()
	{
		return myMin;
	}


	template<typename T>
	inline const Vector2<T>& AABB2D<T>::Min() const
	{
		return myMin;
	}

	template<typename T>
	inline Vector2<T>& AABB2D<T>::Max()
	{
		return myMax;
	}

	template<typename T>
	inline const Vector2<T>& AABB2D<T>::Max() const
	{
		return myMax;
	}

	template<typename T>
	inline bool AABB2D<T>::IsInside(const Vector2<T>& aPosition) const
	{
		return
			aPosition.x >= myMin.x && aPosition.x <= myMax.x &&
			aPosition.y >= myMin.y && aPosition.y <= myMax.y;
	}
};
