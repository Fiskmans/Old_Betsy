#pragma once
#include <initializer_list>
#include "../../Tools/Logger.h"
#include <math.h>
#include <iostream>

#define CU CommonUtilities
#define V3F CommonUtilities::Vector3<float>

namespace CommonUtilities
{
	template<class T>
	class Vector4;	
	template<class T>
	class Vector2;
		
	template<class T>
	class Vector3
	{
	public:
		Vector3<T>();
		Vector3<T>(const T aXValue, const T aYValue, const T aZValue);
		Vector3<T>(const std::initializer_list<T>& anInitList);
		Vector3<T>(const Vector3<T>& aVector) = default;
		Vector3<T>(const Vector2<T>& aVector, const T aZValue);
		Vector3<T>(const Vector4<T>& aVector);
		~Vector3<T>() = default;

		T x;
		T y;
		T z;

		Vector3<T> &operator=(const Vector3 &aVector) = default;
		bool operator==(const Vector3 &aVector) const;
		bool operator!=(const Vector3 &aVector) const;

		T Length() const;
		T LengthSqr() const;
		T Distance(const Vector3<T> aVector) const;
		T DistanceSqr(const Vector3<T> aVector) const;
		Vector3<T> GetNormalized() const;
		void Normalize();
		bool IsNormalized() const;
		bool IsZero() const;

		const Vector3<T>& RotateX(const double aRad); //Should return reference?
		Vector3<T> RotatedX(const double aRad) const; 
		const Vector3<T>& RotateAroundPointX(const Vector3<T>& aVector, const double aRad); 
		Vector3<T> RotatedAroundPointX(const Vector3<T>& aVector, const double aRad) const; 

		const Vector3<T>& RotateY(const double aRad); 
		Vector3<T> RotatedY(const double aRad) const; 
		const Vector3<T>& RotateAroundPointY(const Vector3<T>& aVector, const double aRad);
		Vector3<T> RotatedAroundPointY(const Vector3<T>& aVector, const double aRad) const; 

		const Vector3<T>& RotateZ(const double aRad); 
		Vector3<T> RotatedZ(const double aRad) const; 
		const Vector3<T>& RotateAroundPointZ(const Vector3<T>& aVector, const double aRad); 
		Vector3<T> RotatedAroundPointZ(const Vector3<T>& aVector, const double aRad) const; 

		Vector3<T> Reflected(const Vector3<T>& aNormal) const;

		T Dot(const Vector3<T>& aVector) const;

		Vector3<T> Cross(const Vector3<T>& aVector) const;

		std::string ToString();

		T& operator[](const unsigned int anIndex);
		const T& operator[](const unsigned int anIndex) const;

		T* begin();
		const T* begin() const;

		T* end();
		const T* end() const;
	};


	//**************************************************************************************

	template<class T>
	inline Vector3<T>::Vector3()
		:x(0),
		y(0),
		z(0)
	{

	}

	template<class T>
	inline Vector3<T>::Vector3(const T aXValue, const T aYValue, const T aZValue)
		:x(aXValue),
		y(aYValue),
		z(aZValue)
	{

	}

	template<class T>
	inline Vector3<T>::Vector3(const std::initializer_list<T>& anInitList) :
		x(T()),
		y(T()),
		z(T())
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

		default:
			SYSERROR("Initializer list for Vector3 is faulty.","")

		}
	}

	template<class T>
	inline Vector3<T>::Vector3(const Vector2<T>& aVector, const T aZValue) :
		x(aVector.x),
		y(aVector.y),
		z(aZValue)
	{
	}
	
	template<class T>
	inline Vector3<T>::Vector3(const Vector4<T>& aVector):
		x(aVector.x),
		y(aVector.y),
		z(aVector.z)
	{
	}

#pragma region Operators
	template<class T>
	inline Vector3<T> operator*(const Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		return Vector3<T>(aVector.x * anotherVector.x, aVector.y * anotherVector.y, aVector.z * anotherVector.z);
	}

	template<class T>
	inline Vector3<T> operator/(const Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		if (anotherVector.x == 0 || anotherVector.y == 0 || anotherVector.z == 0)
		{
			SYSERROR("Division by 0 in Vector3!","");
		}
		return Vector3<T>(aVector.x / anotherVector.x, aVector.y / anotherVector.y, aVector.z / anotherVector.z);
	}

	template<class T>
	inline Vector3<T> operator+(const Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		return Vector3<T>(aVector.x + anotherVector.x, aVector.y + anotherVector.y, aVector.z + anotherVector.z);
	}

	template<class T>
	inline Vector3<T> operator-(const Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		return Vector3<T>(aVector.x - anotherVector.x, aVector.y - anotherVector.y, aVector.z - anotherVector.z);
	}

	template<class T>
	inline Vector3<T> operator*=(Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		aVector.x *= anotherVector.x;
		aVector.y *= anotherVector.y;
		aVector.z *= anotherVector.z;
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator/=(Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		assert(anotherVector.x > 0 && anotherVector.y > 0 && anotherVector.z > 0 && "Division by 0!");
		aVector.x /= anotherVector.x;
		aVector.y /= anotherVector.y;
		aVector.z /= anotherVector.z;
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator+=(Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		aVector.x += anotherVector.x;
		aVector.y += anotherVector.y;
		aVector.z += anotherVector.z;
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator-=(Vector3<T> &aVector, const Vector3<T> &anotherVector)
	{
		aVector.x -= anotherVector.x;
		aVector.y -= anotherVector.y;
		aVector.z -= anotherVector.z;
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator*(const Vector3<T> &aVector, const T aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	template<class T>
	inline Vector3<T> operator*(const T aScalar, const Vector3<T> &aVector)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	template<class T>
	inline Vector3<T> operator/(const Vector3<T> &aVector, const T aScalar)
	{
		if (aScalar == 0)
		{
			SYSERROR("Division by 0 in Vector3!","");
		}
		double divNum = 1.0 / static_cast<double>(aScalar);

		return Vector3<T>(static_cast<T>(aVector.x * divNum), static_cast<T>(aVector.y * divNum), static_cast<T>(aVector.z * divNum));
	}

	template<class T>
	inline Vector3<T> operator+(const Vector3<T> &aVector, const T aScalar)
	{
		return Vector3<T>(aVector.x + aScalar, aVector.y + aScalar, aVector.z + aScalar);
	}

	template<class T>
	inline Vector3<T> operator-(const Vector3<T> &aVector, const T aScalar)
	{
		return Vector3<T>(aVector.x - aScalar, aVector.y - aScalar, aVector.z - aScalar);
	}

	template<class T>
	inline Vector3<T> operator*=(Vector3<T> &aVector, const T aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator/=(Vector3<T> &aVector, const T aScalar)
	{
		if (aScalar == 0)
		{
			SYSERROR("Division by 0 in Vector3!","");
		}

		double divNum = 1.0 / static_cast<double>(aScalar);

		aVector.x = static_cast<T>(aVector.x * divNum);
		aVector.y = static_cast<T>(aVector.y * divNum);
		aVector.z = static_cast<T>(aVector.z * divNum);
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator+=(Vector3<T> &aVector, const T aScalar)
	{
		aVector.x += aScalar;
		aVector.y += aScalar;
		aVector.z += aScalar;
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator-=(Vector3<T> &aVector, const T aScalar)
	{
		aVector.x -= aScalar;
		aVector.y -= aScalar;
		aVector.z -= aScalar;
		return aVector;
	}

	template<class T>
	inline Vector3<T> operator-(Vector3<T> aVector)
	{
		return Vector3<T>(-aVector.x, -aVector.y, -aVector.z);
	}

	template<class T>
	inline bool Vector3<T>::operator==(const Vector3& aVector) const
	{
		return (x == aVector.x && y == aVector.y && z == aVector.z);
	}

	template<class T>
	inline bool Vector3<T>::operator!=(const Vector3& aVector) const
	{
		return !((*this) == aVector);
	}

#pragma endregion

	template<class T>
	inline T Vector3<T>::Length() const
	{
		return static_cast<T>(sqrt((x * x) + (y * y) + (z * z)));
	}

	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	template<class T>
	inline T Vector3<T>::Distance(const Vector3<T> aVector) const
	{
		return ((*this) - aVector).Length();
	}

	template<class T>
	inline T Vector3<T>::DistanceSqr(const Vector3<T> aVector) const
	{
		return ((*this) - aVector).LengthSqr();
	}

	template<class T>
	inline void Vector3<T>::Normalize()
	{
		T length = Length();

		if (length > 0)
		{
			(*this) /= length;
			return;
		}

		//SYSWARNING("Normalization of zero vector in Vector3!"); 
	}

	template<class T>
	inline bool Vector3<T>::IsNormalized() const
	{
		return (LengthSqr() == T(1));
	}

	template<class T>
	inline bool Vector3<T>::IsZero() const
	{
		return (*this) == Vector3<T>(0, 0, 0);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		T length = Length();

		if (length > 0)
		{
			return (*this) / length;
		}

		//SYSWARNING("Normalization of zero vector in Vector3!");
		return Vector3<T>();
	}

#pragma region Rotate X
	template<class T>
	inline const Vector3<T>& Vector3<T>::RotateX(const double aRad)
	{
		double c = cos(aRad);
		double s = sin(aRad);

		T yCopy = y;
		y = static_cast<T>(y * c - z * s);
		z = static_cast<T>(yCopy * s + z * c);
		return (*this);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::RotatedX(const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);

		return Vector3<T>(x, static_cast<T>(y * c + z * s), static_cast<T>(y * s + z * c));
	}

	template<class T>
	inline const Vector3<T>& Vector3<T>::RotateAroundPointX(const Vector3<T>& aVector, const double aRad)
	{
		y -= aVector.y;
		z -= aVector.z;
		RotateX(aRad);
		y += aVector.y;
		z += aVector.z;

		return (*this);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::RotatedAroundPointX(const Vector3<T>& aVector, const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);
		T movedY = y - aVector.y;
		T movedZ = z - aVector.z;

		return Vector3<T>(x, static_cast<T>((movedY * c + movedZ * s) + aVector.y), static_cast<T>((movedY * s + movedZ * c) + aVector.z));
	}

#pragma endregion

#pragma region Rotate Y
	template<class T>
	inline const Vector3<T>& Vector3<T>::RotateY(const double aRad)
	{
		double c = cos(aRad);
		double s = sin(aRad);

		T xCopy = x;
		x = static_cast<T>(x * c + z * s);
		z = static_cast<T>(-xCopy * s + z * c);
		return (*this);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::RotatedY(const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);

		return Vector3<T>(static_cast<T>(x * c + z * s), y, static_cast<T>(-x * s + z * c));
	}

	template<class T>
	inline const Vector3<T>& Vector3<T>::RotateAroundPointY(const Vector3<T>& aVector, const double aRad)
	{
		x -= aVector.x;
		z -= aVector.z;
		RotateY(aRad);
		x += aVector.x;
		z += aVector.z;

		return (*this);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::RotatedAroundPointY(const Vector3<T>& aVector, const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);
		T movedX = x - aVector.x;
		T movedZ = z - aVector.z;

		return Vector3<T>(static_cast<T>((movedX * c + movedZ * s) + aVector.x), y, static_cast<T>((-movedX * s + movedZ * c) + aVector.z));
	}
#pragma endregion

#pragma region Rotate Z
	template<class T>
	inline const Vector3<T>& Vector3<T>::RotateZ(const double aRad)
	{
		double c = cos(aRad);
		double s = sin(aRad);

		T xCopy = x;
		x = static_cast<T>(x * c - y * s);
		y = static_cast<T>(y * c + xCopy * s);
		return (*this);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::RotatedZ(const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);

		return Vector3<T>(static_cast<T>(x * c - y * s), static_cast<T>(y * c + x * s), z);
	}

	template<class T>
	inline const Vector3<T>& Vector3<T>::RotateAroundPointZ(const Vector3<T>& aVector, const double aRad)
	{
		x -= aVector.x;
		y -= aVector.y;
		RotateZ(aRad);
		x += aVector.x;
		y += aVector.y;

		return (*this);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::RotatedAroundPointZ(const Vector3<T>& aVector, const double aRad) const
	{
		double c = cos(aRad);
		double s = sin(aRad);
		T xCopy = x - aVector.x;
		T yCopy = y - aVector.y;

		return Vector3<T>(static_cast<T>((xCopy * c - yCopy * s) + aVector.x), static_cast<T>((yCopy * c + xCopy * s) + aVector.y), z);
	}
#pragma endregion

	template<class T>
	inline Vector3<T> Vector3<T>::Reflected(const Vector3<T>& aNormal) const
	{
		return (*this) - static_cast<T>(2) * aNormal.Dot(*this) * aNormal;
	}

	template<class T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		return Vector3<T>((y * aVector.z) - (z * aVector.y), (z * aVector.x) - (x * aVector.z), (x * aVector.y) - (y * aVector.x));
	}

	template<class T>
	inline std::string Vector3<T>::ToString()
	{
		return  "X:" + std::to_string(x) + " Y:" + std::to_string(y) + " Z:" + std::to_string(z);
	}

	template<class T>
	inline T& Vector3<T>::operator[](const unsigned int index)
	{
		if (index > 3 || index < 0)
		{
			SYSERROR("Index out of range!","");
		}

		return (*(begin() + index));
	}
	
	template<class T>
	inline const T& Vector3<T>::operator[](const unsigned int index) const
	{
		if (index > 3 || index < 0)
		{
			SYSERROR("Index out of range!","");
		}

		return (*(begin() + index));
	}

	template<class T>
	inline T* Vector3<T>::begin()
	{
		return &x;
	}

	template<class T>
	inline const T* Vector3<T>::begin() const
	{
		return &x;
	}

	template<class T>
	inline const T* Vector3<T>::end() const
	{
		return (&z) + 1;
	}

	template<class T>
	inline T* Vector3<T>::end()
	{
		return (&z) + 1;
	}
}

template<class T>
std::istream& operator>>(std::istream& aStream, CU::Vector3<T>& aVector)
{
	return aStream >> aVector.x >> aVector.y >> aVector.z;
}