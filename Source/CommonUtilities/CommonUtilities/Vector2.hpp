#pragma once
#include <initializer_list>
#include "../../Tools/Logger.h"
#include <math.h>
#include "../../Tools/FastSqrt.h"

#define CU CommonUtilities
#define V2F CommonUtilities::Vector2<float>

namespace CommonUtilities
{
	template<class T>
	class Vector3;
	template<class T>
	class Vector4;

	template<class T>
	class Vector2
	{
	public:
		Vector2<T>();
		Vector2<T>(const T aXValue, const T aYValue);
		Vector2<T>(const Vector2<T>& aVector) = default;
		Vector2<T>(const Vector3<T>& aVector);
		Vector2<T>(const Vector4<T>& aVector);
		Vector2<T>(const std::initializer_list<T>& anInitList);
		~Vector2<T>() = default;

		T x;
		T y;

		Vector2<T> &operator=(const Vector2<T> &aVector) = default;
		bool operator==(const Vector2<T>& aVector) const;
		bool operator!=(const Vector2<T>& aVector) const;

		T Length() const;
		T FastLength() const;
		T LengthSqr() const;
		T Distance(const Vector2<T> &aVector) const;
		T FastDistance(const Vector2<T> &aVector) const;
		T DistanceSqr(const Vector2<T> &aVector) const;
		void Normalize();
		Vector2<T> GetNormalized() const;
		void FastNormalize();
		Vector2<T> GetFastNormalized() const;
		bool IsNormalized() const;
		bool IsZero() const;

		void Rotate(const double aRad);
		Vector2<T> Rotated(const double aRad) const;
		void RotateAroundPoint(const Vector2<T> &aVector, const double aRad);
		Vector2<T> RotatedAroundPoint(const Vector2<T> &aVector, const double aRad) const;

		Vector2<T> Reflected(const Vector2<T>& aNormal) const;

		T Dot(const Vector2<T> &aVector) const;
	};

	//**************************************************************************************

	template<class T>
	inline Vector2<T>::Vector2()
		:x(0),
		y(0)
	{

	}

	template<class T>
	inline Vector2<T>::Vector2(const T aXValue, const T aYValue)
		:x(aXValue),
		y(aYValue)
	{
	}

	template<class T>
	inline Vector2<T>::Vector2(const Vector3<T>& aVector) :
		x(aVector.x),
		y(aVector.y)
	{
	}

	template<class T>
	inline Vector2<T>::Vector2(const Vector4<T>& aVector) :
		x(aVector.x),
		y(aVector.y)
	{
	}

	template<class T>
	inline Vector2<T>::Vector2(const std::initializer_list<T>& anInitList) :
		x(T()),
		y(T())
	{
		switch (anInitList.size())
		{
		case 0:
			break;

		case 1:
			x = *anInitList.begin();
			break;

		case 2:
			x = *anInitList.begin();
			y = *(anInitList.begin() + 1);
			break;

		default:
			SYSERROR("Initializer list for Vector2 is faulty.","")
		}
	}

#pragma region Operators
	template<class T>
	inline Vector2<T> operator*(const Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		return Vector2<T>(aVector.x * anotherVector.x, aVector.y * anotherVector.y);
	}

	template<class T>
	inline Vector2<T> operator/(const Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		if (anotherVector.x == 0 || anotherVector.y == 0)
		{
			SYSERROR("Division by 0 in Vector2!","");
		}

		return Vector2<T>(aVector.x / anotherVector.x, aVector.y / anotherVector.y);
	}

	template<class T>
	inline Vector2<T> operator+(const Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		return Vector2<T>(aVector.x + anotherVector.x, aVector.y + anotherVector.y);
	}

	template<class T>
	inline Vector2<T> operator-(const Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		return Vector2<T>(aVector.x - anotherVector.x, aVector.y - anotherVector.y);
	}

	template<class T>
	inline Vector2<T> operator*=(Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		aVector.x *= anotherVector.x;
		aVector.y *= anotherVector.y;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator/=(Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		if (anotherVector.x == 0 || anotherVector.y == 0)
		{
			SYSERROR("Division by 0 in Vector2!","");
		}

		aVector.x /= anotherVector.x;
		aVector.y /= anotherVector.y;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator+=(Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		aVector.x += anotherVector.x;
		aVector.y += anotherVector.y;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator-=(Vector2<T> &aVector, const Vector2<T> &anotherVector)
	{
		aVector.x -= anotherVector.x;
		aVector.y -= anotherVector.y;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator*(const Vector2<T> &aVector, const T aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}

	template<class T>
	inline Vector2<T> operator*(const T aScalar, const Vector2<T> &aVector)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}

	template<class T>
	inline Vector2<T> operator/(const Vector2<T> &aVector, const T aScalar)
	{
		if (aScalar == 0)
		{
			SYSERROR("Division by 0 in Vector2!","");
		}

		return Vector2<T>(aVector.x / aScalar, aVector.y / aScalar);
	}

	template<class T>
	inline Vector2<T> operator+(const Vector2<T> &aVector, const T aScalar)
	{
		return Vector2<T>(aVector.x + aScalar, aVector.y + aScalar);
	}

	template<class T>
	inline Vector2<T> operator-(const Vector2<T> &aVector, const T aScalar)
	{
		return Vector2<T>(aVector.x - aScalar, aVector.y - aScalar);
	}

	template<class T>
	inline Vector2<T> operator*=(Vector2<T> &aVector, const T aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator/=(Vector2<T> &aVector, const T aScalar)
	{
		if (aScalar == 0)
		{
			SYSERROR("Division by 0 in Vector2!","");
		}

		aVector.x /= aScalar;
		aVector.y /= aScalar;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator+=(Vector2<T> &aVector, const T aScalar)
	{
		aVector.x += aScalar;
		aVector.y += aScalar;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator-=(Vector2<T> &aVector, const T aScalar)
	{
		aVector.x -= aScalar;
		aVector.y -= aScalar;
		return aVector;
	}

	template<class T>
	inline Vector2<T> operator-(Vector2<T> aVector)
	{
		return Vector2<T>(-aVector.x, -aVector.y);
	}

	template<class T>
	inline bool Vector2<T>::operator==(const Vector2<T>& aVector) const
	{
		return (x == aVector.x && y == aVector.y);
	}

	template<class T>
	inline bool Vector2<T>::operator!=(const Vector2<T>& aVector) const
	{
		return !((*this) == aVector);
	}

#pragma endregion

	template<class T>
	inline T Vector2<T>::Length() const
	{
		return static_cast<T>(sqrt((x * x) + (y * y)));
	}

	template<class T>
	inline T Vector2<T>::FastLength() const
	{
		T sqrLength = LengthSqr();
		return static_cast<T>(sqrLength * FastInverseSquareRoot(static_cast<float> (sqrLength)));
	}

	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return (x * x) + (y * y);
	}

	template<class T>
	inline T Vector2<T>::Distance(const Vector2<T>& aVector) const
	{
		return ((*this) - aVector).Length();
	}

	template<class T>
	inline T Vector2<T>::FastDistance(const Vector2<T>& aVector) const
	{
		return ((*this) - aVector).FastLength();
	}

	template<class T>
	inline T Vector2<T>::DistanceSqr(const Vector2<T>& aVector) const
	{
		return ((*this) - aVector).LengthSqr();
	}

	template<class T>
	inline void Vector2<T>::Normalize()
	{
		T length = Length();

		if (length > 0)
		{
			(*this) *= (T(1) / length);
			return;
		}

		SYSWARNING("Normalization of zero vector in Vector2!","");
	}

	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		T length = Length();

		if (length > 0)
		{
			return (*this) * (T(1) / length);
		}

		SYSWARNING("Normalization of zero vector in Vector2!",""); 
		return Vector2<T>();
	}

	template<class T>
	inline void Vector2<T>::FastNormalize()
	{
		T sqrLength = LengthSqr();

		if (sqrLength > 0)
		{
			(*this) *= FastInverseSquareRoot(static_cast<float>(sqrLength));
			return;
		}
	}

	template<class T>
	inline Vector2<T> Vector2<T>::GetFastNormalized() const
	{
		T sqrLength = LengthSqr();

		if (sqrLength > 0)
		{
			return (*this) * FastInverseSquareRoot(static_cast<float>(sqrLength));
		}

		return Vector2<T>();
	}

	template<class T>
	inline bool Vector2<T>::IsNormalized() const
	{
		return (LengthSqr() == T(1));
	}

	template<class T>
	inline bool Vector2<T>::IsZero() const
	{
		return (*this) == Vector2<T>(0, 0);
	}

	template<class T>
	inline void Vector2<T>::Rotate(double aRad)
	{
		double c = cos(aRad);
		double s = sin(aRad);

		T xCopy = x;
		x = static_cast<T>(x * c - y * s);
		y = static_cast<T>(y * c + xCopy * s);
	}

	template<class T>
	inline Vector2<T> Vector2<T>::Rotated(double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);

		return Vector2(static_cast<T>(x * c - y * s), static_cast<T>(y * c + x * s));
	}

	template<class T>
	inline void Vector2<T>::RotateAroundPoint(const Vector2<T> &aVector, double aRad)
	{
		x -= aVector.x;
		y -= aVector.y;
		Rotate(aRad);
		x += aVector.x;
		y += aVector.y;

		return (*this);
	}

	template<class T>
	inline Vector2<T> Vector2<T>::RotatedAroundPoint(const Vector2<T> &aVector, double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);
		T tempX = x - aVector.x;
		T tempY = y - aVector.y;

		return Vector2(static_cast<T>((tempX * c - tempY * s) + aVector.x), static_cast<T>((tempY * c + tempX * s) + aVector.y));
	}

	template<class T>
	inline Vector2<T> Vector2<T>::Reflected(const Vector2<T>& aNormal) const
	{
		return (*this) - static_cast<T>(2) * aNormal.Dot(*this) * aNormal;
	}


	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T> &aVector) const
	{
		return (x * aVector.x) + (y * aVector.y);
	}
}

