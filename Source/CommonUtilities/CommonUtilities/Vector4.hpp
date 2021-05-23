#pragma once
#include <initializer_list>
#include "../../Tools/Logger.h"
#include <math.h>

#define CU CommonUtilities
#define V4F CommonUtilities::Vector4<float>

namespace CommonUtilities
{
	template<class T>
	class Vector2;
	template<class T>
	class Vector3;

	template<class T>
	class Vector4
	{
	public:
		Vector4<T>();
		Vector4<T>(const T aXValue, const T aYValue, const T aZValue, const T aWValue);
		Vector4<T>(const Vector4<T> &aVector) = default;
		Vector4<T>(const Vector3<T> &aVector, const T aWValue);
		Vector4<T>(const Vector2<T> &aVector, const T aZValue, const T aWValue);
		Vector4<T>(const std::initializer_list<T>& anInitList);
		~Vector4<T>() = default;

		T x;
		T y;
		T z;
		T w;

		Vector4<T> &operator=(const Vector4<T> &aVector) = default;
		bool operator==(const Vector4<T> &aVector) const;
		bool operator!=(const Vector4<T> &aVector) const;
		T& operator[](size_t aIndex);
		T operator[](size_t aIndex) const;

		T Length() const;
		T LengthSqr() const;
		T Distance(const Vector4<T>& aVector) const;
		T DistanceSqr(const Vector4<T>& aVector) const;
		Vector4<T> GetNormalized() const;
		void Normalize();
		bool IsNormalized() const;
		bool IsZero() const;

		Vector4<T> RotateX(const double aRad);
		Vector4<T> RotatedX(const double aRad) const;
		Vector4<T> RotateAroundPointX(const Vector4<T>& aVector, const double aRad);
		Vector4<T> RotatedAroundPointX(const Vector4<T>& aVector, const double aRad) const;
			  
		Vector4<T> RotateY(const double aRad);
		Vector4<T> RotatedY(const double aRad) const;
		Vector4<T> RotateAroundPointY(const Vector4<T>& aVector, const double aRad);
		Vector4<T> RotatedAroundPointY(const Vector4<T>& aVector, const double aRad) const;
			  
		Vector4<T> RotateZ(const double aRad);
		Vector4<T> RotatedZ(const double aRad) const;
		Vector4<T> RotateAroundPointZ(const Vector4<T>& aVector, const double aRad);
		Vector4<T> RotatedAroundPointZ(const Vector4<T>& aVector, const double aRad) const;

		Vector4<T> Reflected(const Vector4<T>& aNormal) const;

		T Dot(const Vector4<T> &aVector) const;

		operator Vector3<T>();
	};


	//**************************************************************************************

	template<class T>
	inline Vector4<T>::Vector4()
		:x(0),
		y(0),
		z(0),
		w(0)
	{

	}

	template<class T>
	inline Vector4<T>::Vector4(const T aXValue, const T aYValue, const T aZValue, const T aWValue)
		:x(aXValue),
		y(aYValue),
		z(aZValue),
		w(aWValue)
	{

	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector3<T>& aVector, const T aWValue) :
		x(aVector.x),
		y(aVector.y),
		z(aVector.z),
		w(aWValue)
	{
	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector2<T>& aVector, const T aZValue, const T aWValue) :
		x(aVector.x),
		y(aVector.y),
		z(aZValue),
		w(aWValue)
	{
	}

	template<class T>
	inline Vector4<T>::Vector4(const std::initializer_list<T>& anInitList) :
		x(T()),
		y(T()),
		z(T()),
		w(T())
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

		case 3:
			x = *anInitList.begin();
			y = *(anInitList.begin() + 1);
			z = *(anInitList.begin() + 2);
			break;

		case 4:
			x = *anInitList.begin();
			y = *(anInitList.begin() + 1);
			z = *(anInitList.begin() + 2);
			w = *(anInitList.begin() + 3);
			break;

		default:
			SYSERROR("Initializer list for Vector4 is faulty.","")

		}
	}

#pragma region Operators

	template<class T>
	inline Vector4<T>::operator Vector3<T>()
	{
		return Vector3<T>(x, y, z);
	}

	template<class T>
	inline Vector4<T> operator*(const Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		return Vector4<T>(aVector.x * anotherVector.x, aVector.y * anotherVector.y, aVector.z * anotherVector.z, aVector.w * anotherVector.w);
	}

	template<class T>
	inline Vector3<T> operator*(const Vector3<T>& aVector, const Vector4<T>& anotherVector)
	{
		return aVector * Vector3<T>(anotherVector);
	}

	template<class T>
	inline Vector3<T> operator+(const Vector3<T>& aVector, const Vector4<T>& anotherVector)
	{
		return aVector + Vector3<T>(anotherVector);
	}

	template<class T>
	inline Vector4<T> operator/(const Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		if (anotherVector.x == 0 || anotherVector.y == 0 || anotherVector.z == 0 || anotherVector.y == 0)
		{
			SYSERROR("Division by 0 in Vector4!","");
		}
		return Vector4<T>(aVector.x / anotherVector.x, aVector.y / anotherVector.y, aVector.z / anotherVector.z, aVector.w / anotherVector.w);
	}

	template<class T>
	inline Vector4<T> operator+(const Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		return Vector4<T>(aVector.x + anotherVector.x, aVector.y + anotherVector.y, aVector.z + anotherVector.z, aVector.w + anotherVector.w);
	}

	template<class T>
	inline Vector4<T> operator-(const Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		return Vector4<T>(aVector.x - anotherVector.x, aVector.y - anotherVector.y, aVector.z - anotherVector.z, aVector.w - anotherVector.w);
	}

	template<class T>
	inline Vector4<T> operator*=(Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		aVector.x *= anotherVector.x;
		aVector.y *= anotherVector.y;
		aVector.z *= anotherVector.z;
		aVector.w *= anotherVector.w;
		return aVector;
	}

	template<class T>
	inline Vector4<T> operator/=(Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		if (anotherVector.x == 0 || anotherVector.y == 0 || anotherVector.z == 0 || anotherVector.y == 0)
		{
			SYSERROR("Division by 0 in Vector4!","");
		}
		aVector.x /= anotherVector.x;
		aVector.y /= anotherVector.y;
		aVector.z /= anotherVector.z;
		aVector.w /= anotherVector.w;
		return aVector;
	}

	template<class T>
	inline Vector4<T> operator+=(Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		aVector.x += anotherVector.x;
		aVector.y += anotherVector.y;
		aVector.z += anotherVector.z;
		aVector.w += anotherVector.w;
		return aVector;
	}

	template<class T>
	inline Vector4<T> operator-=(Vector4<T> &aVector, const Vector4<T> &anotherVector)
	{
		aVector.x -= anotherVector.x;
		aVector.y -= anotherVector.y;
		aVector.z -= anotherVector.z;
		aVector.w -= anotherVector.w;
		return aVector;
	}

	template<class T>
	inline Vector4<T> operator*(const Vector4<T> &aVector, const T aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}

	template<class T>
	inline Vector4<T> operator*(const T &aScalar, const Vector4<T> aVector)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}

	template<class T>
	inline Vector4<T> operator/(const Vector4<T> &aVector, const T aScalar)
	{
		if (aScalar == 0)
		{
			SYSERROR("Division by 0 in Vector4!","");
		}

		double divNum = 1.0 / static_cast<double>(aScalar);

		return Vector4<T>(static_cast<T>(aVector.x * divNum), static_cast<T>(aVector.y * divNum), static_cast<T>(aVector.z * divNum), static_cast<T>(aVector.w * divNum));
	}

	template<class T>
	inline Vector4<T> operator+(const Vector4<T> &aVector, const T aScalar)
	{
		return Vector4<T>(aVector.x + aScalar, aVector.y + aScalar, aVector.z + aScalar, aVector.w + aScalar);
	}

	template<class T>
	inline Vector4<T> operator-(const Vector4<T> &aVector, const T aScalar)
	{
		return Vector4<T>(aVector.x - aScalar, aVector.y - aScalar, aVector.z - aScalar, aVector.w - aScalar);
	}

	template<class T>
	inline Vector4<T> operator*=(Vector4<T> &aVector, const T aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
		return aVector;
	}

	template<class T>
	inline Vector4<T> operator/=(Vector4<T> &aVector, const T aScalar)
	{
		if (aScalar == 0)
		{
			SYSERROR("Division by 0 in Vector4!","");
		}

		double divNum = 1.0 / static_cast<double>(aScalar);

		aVector.x = static_cast<T>(aVector.x * divNum);
		aVector.y = static_cast<T>(aVector.y * divNum);
		aVector.z = static_cast<T>(aVector.z * divNum);
		aVector.w = static_cast<T>(aVector.w * divNum);
		return aVector;
	}

	template<class T>
	inline Vector4<T> operator+=(Vector4<T> &aVector, const T aScalar)
	{
		aVector.x += aScalar;
		aVector.y += aScalar;
		aVector.z += aScalar;
		aVector.w += aScalar;
		return aVector;
	}

	template<class T>
	inline Vector4<T> operator-=(Vector4<T> &aVector, const T aScalar)
	{
		aVector.x -= aScalar;
		aVector.y -= aScalar;
		aVector.z -= aScalar;
		aVector.w -= aScalar;
		return aVector;
	}

	template<class T>
	inline T& Vector4<T>::operator[](size_t aIndex)
	{
		switch (aIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw - 1;
		}
		return x;
	}

	template<class T>
	inline T Vector4<T>::operator[](size_t aIndex) const
	{
		switch (aIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw - 1;
		}
		return x;
	}

	template<class T>
	inline Vector4<T> operator-(Vector4<T> aVector)
	{
		return Vector4<T>(-aVector.x, -aVector.y, -aVector.z, -aVector.w);
	}

	template<class T>
	inline bool Vector4<T>::operator==(const Vector4<T>& aVector) const
	{
		return (x == aVector.x && y == aVector.y && z == aVector.z && w == aVector.w);
	}

	template<class T>
	inline bool Vector4<T>::operator!=(const Vector4<T>& aVector) const
	{
		return !((*this) == aVector);
	}
#pragma endregion

	template<class T>
	inline T Vector4<T>::Length() const
	{
		return static_cast<T>(sqrt((x * x) + (y * y) + (z * z) + (w * w)));
	}

	template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
		return (x * x) + (y * y) + (z * z) + (w * w);
	}

	template<class T>
	inline T Vector4<T>::Distance(const Vector4<T>& aVector) const
	{
		return ((*this) - aVector).Length();
	}

	template<class T>
	inline T Vector4<T>::DistanceSqr(const Vector4<T>& aVector) const
	{
		return ((*this) - aVector).LengthSqr();
	}

	template<class T>
	inline void Vector4<T>::Normalize()
	{
		T length = Length();

		if (length > 0)
		{
			(*this) /= length;
			return;
		}

		//SYSWARNING("Normalization of zero vector in Vector4!");
	}

	template<class T>
	inline bool Vector4<T>::IsNormalized() const
	{
		return (LengthSqr() == T(1));
	}

	template<class T>
	inline bool Vector4<T>::IsZero() const
	{
		return (*this) == Vector4<T>(0, 0, 0, 0);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		T length = Length();

		if (length > 0)
		{
			return (*this) / length;
		}

		//SYSWARNING("Normalization of zero vector in Vector4!"); 
		return Vector4<T>();
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotateX(const double aRad)
	{
		double c = cos(aRad);
		double s = sin(aRad);

		T yCopy = y;
		y = static_cast<T>(y * c - z * s);
		z = static_cast<T>(yCopy * s + z * c);
		return (*this);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotatedX(const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);

		return Vector4<T>(x, static_cast<T>(y * c + z * s), static_cast<T>(y * s + z * c), w);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotateAroundPointX(const Vector4<T>& aVector, const double aRad)
	{
		y -= aVector.y;
		z -= aVector.z;
		RotateX(aRad);
		y += aVector.y;
		z += aVector.z;

		return (*this);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotatedAroundPointX(const Vector4<T>& aVector, const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);
		T movedY = y - aVector.y;
		T movedZ = z - aVector.z;

		return Vector4<T>(x, static_cast<T>((movedY * c + movedZ * s) + aVector.y), static_cast<T>((movedY * s + movedZ * c) + aVector.z), w);

	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotateY(const double aRad)
	{
		double c = cos(aRad);
		double s = sin(aRad);

		T xCopy = x;
		x = static_cast<T>(x * c + z * s);
		z = static_cast<T>(-xCopy * s + z * c);
		return (*this);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotatedY(const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);

		return Vector4<T>(static_cast<T>(x * c + z * s), y, static_cast<T>(-x * s + z * c), w);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotateAroundPointY(const Vector4<T>& aVector, const double aRad)
	{
		x -= aVector.x;
		z -= aVector.z;
		RotateY(aRad);
		x += aVector.x;
		z += aVector.z;

		return (*this);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotatedAroundPointY(const Vector4<T>& aVector, const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);
		T movedX = x - aVector.x;
		T movedZ = z - aVector.z;

		return Vector4<T>(static_cast<T>((movedX * c + movedZ * s) + aVector.x), y, static_cast<T>((-movedX * s + movedZ * c) + aVector.z), w);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotateZ(const double aRad)
	{
		double c = cos(aRad);
		double s = sin(aRad);

		T xCopy = x;
		x = static_cast<T>(x * c - y * s);
		y = static_cast<T>(y * c + xCopy * s);
		return (*this);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotatedZ(const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);

		return Vector4<T>(static_cast<T>(x * c - y * s), static_cast<T>(y * c + x * s), z, w);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotateAroundPointZ(const Vector4<T>& aVector, const double aRad)
	{
		x -= aVector.x;
		y -= aVector.y;
		RotateZ(aRad);
		x += aVector.x;
		y += aVector.y;

		return (*this);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::RotatedAroundPointZ(const Vector4<T>& aVector, const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);
		T xCopy = x - aVector.x;
		T yCopy = y - aVector.y;

		return Vector4<T>(static_cast<T>((xCopy * c - yCopy * s) + aVector.x), static_cast<T>((yCopy * c + xCopy * s) + aVector.y), z);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::Reflected(const Vector4<T>& aNormal) const
	{
		return (*this) - static_cast<T>(2) * aNormal.Dot(*this) * aNormal;
	}

	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T> &aVector) const
	{
		return (x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w);
	}

}