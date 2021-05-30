#pragma once
#include "../../Tools/Logger.h"

namespace CommonUtilities
{
	template <class T>
	class Vector3;
	template <class T>
	class Vector4;

	template <class T>
	class Matrix4x4
	{
	public:
		Matrix4x4(); //Creates an identity matrix
		Matrix4x4(const Matrix4x4<T> &aMatrix);
		Matrix4x4(const std::initializer_list<T>& anInitList);
		~Matrix4x4() = default;

		static Matrix4x4<T> CreateRotationAroundX(const T anAngle);
		static Matrix4x4<T> CreateRotationAroundY(const T anAngle);
		static Matrix4x4<T> CreateRotationAroundZ(const T anAngle);
		static Matrix4x4<T> CreateRotationAroundPointX(const T anAngle, const Vector4<T> &aPoint);
		static Matrix4x4<T> CreateRotationAroundPointY(const T anAngle, const Vector4<T> &aPoint);
		static Matrix4x4<T> CreateRotationAroundPointZ(const T anAngle, const Vector4<T> &aPoint);
		
		static Matrix4x4<T> CreateRotationFromDirection(const Vector3<T>& aDirection);

		static Matrix4x4<T> Identity();

		void RotateAroundX(const T anAngle);
		void RotateAroundY(const T anAngle);
		void RotateAroundZ(const T anAngle);
		void RotateAroundPointX(const T anAngle, const Vector4<T> &aPoint);
		void RotateAroundPointY(const T anAngle, const Vector4<T> &aPoint);
		void RotateAroundPointZ(const T anAngle, const Vector4<T> &aPoint);

		static Matrix4x4<T> Transpose(const Matrix4x4<T> &aMatrixToTranspose);

		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T> &aTransform);
		static Matrix4x4<T> GetRealInverse(const Matrix4x4<T>& aTransform);
		//Operators
		Matrix4x4<T> &operator=(const Matrix4x4<T> &aMatrix);

		Matrix4x4<T> operator+(const Matrix4x4<T> &aMatrix) const;
		Matrix4x4<T>& operator+=(const Matrix4x4<T> &aMatrix);

		Matrix4x4<T> operator-(const Matrix4x4<T> &aMatrix) const;
		Matrix4x4<T> operator-=(const Matrix4x4<T> &aMatrix);

		Matrix4x4<T> operator*(const Matrix4x4<T> &aMatrix) const;
		Matrix4x4<T>& operator*=(const Matrix4x4<T> &aMatrix);

		Matrix4x4<T> operator*(const T aScalar) const;
		Matrix4x4<T>& operator*=(const T aScalar);

		Matrix4x4<T> operator/(const T aScalar) const;
		Matrix4x4<T>& operator/=(const T aScalar);

		//Vector4<T> &operator*(const Vector4<T> &aVector, const Matrix4x4<T> &aMatrix) //has to be non-member

		Matrix4x4<T> operator-(void);

		Vector4<T> Row(size_t aIndex) const;
		void AssignRow(size_t aIndex,const Vector4<T>& aVector);
		Vector4<T> Column(size_t aIndex) const;
		void AssignColumn(size_t aIndex, const Vector4<T>& aVector);

		bool operator==(const Matrix4x4<T> &aMatrix) const;
		bool operator!=(const Matrix4x4<T> &aMatrix) const;

		T& operator()(const unsigned int aRow, const unsigned int aColumn);
		const T& operator()(const unsigned int aRow, const unsigned int aColumn) const;

		T& operator[](const unsigned int anIndex);
		const T& operator[](const unsigned int anIndex) const;

	private:

		union
		{
			T myData[4][4];
			T myElements[16];
		};
	};

	//*****************************************************************************

	template<class T>
	inline Matrix4x4<T>::Matrix4x4()
		:myElements{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }
	{
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T> &aMatrix) : myElements{}
	{
		for (size_t index = 0; index < sizeof(myElements) / sizeof(T); index++)
		{
			myElements[index] = aMatrix.myElements[index];
		}
	}


	template<class T>
	inline Vector4<T> Matrix4x4<T>::Row(size_t aIndex) const
	{
		assert(aIndex < 4 && "accessing row out of range");
		return Vector4<T>(myData[aIndex][0], myData[aIndex][1], myData[aIndex][2], myData[aIndex][3]);
	}

	template<class T>
	inline void Matrix4x4<T>::AssignRow(size_t aIndex, const Vector4<T>& aVector)
	{
		assert(aIndex < 4 && "accessing row out of range");
		myData[aIndex][0] = aVector.x; 
		myData[aIndex][1] = aVector.y;
		myData[aIndex][2] = aVector.z;
		myData[aIndex][3] = aVector.w;
	}

	template<class T>
	inline Vector4<T> Matrix4x4<T>::Column(size_t aIndex) const
	{
		assert(aIndex < 4 && "accessing row out of range");
		return Vector4<T>(myData[0][aIndex], myData[1][aIndex], myData[2][aIndex], myData[3][aIndex]);
	}

	template<class T>
	inline void Matrix4x4<T>::AssignColumn(size_t aIndex, const Vector4<T>& aVector)
	{
		assert(aIndex < 4 && "accessing row out of range");
		myData[0][aIndex] = aVector.x;
		myData[1][aIndex] = aVector.y;
		myData[2][aIndex] = aVector.z;
		myData[3][aIndex] = aVector.w;
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const std::initializer_list<T>& anInitList) : myElements{}
	{
		assert(anInitList.size() == sizeof(myElements) / sizeof(T));

		for (size_t i = 0; i < sizeof(myElements) / sizeof(T); ++i)
		{
			myElements[i] = *(anInitList.begin() + i);
		}
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return { 1,  0, 0, 0,
				 0,  c, s, 0,
				 0, -s, c, 0,
				 0,  0, 0, 1 };
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return { c, 0, -s, 0,
				 0, 1,  0, 0,
				 s, 0,  c, 0,
				 0, 0,  0, 1 };
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return { c, s, 0, 0,
				-s, c, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 };
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointX(const T anAngle, const Vector4<T> &aPoint)
	{
		return 
			Matrix4x4<T>({ 1, 0, 0,  aPoint.x, 0, 1, 0,  aPoint.y, 0, 0, 1,  aPoint.z, 0, 0, 0, 1 }) *
			CreateRotationAroundX(anAngle) * 
			Matrix4x4<T>({ 1, 0, 0, -aPoint.x, 0, 1, 0, -aPoint.y, 0, 0, 1, -aPoint.z, 0, 0, 0, 1 });
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointY(const T anAngle, const Vector4<T> &aPoint)
	{
		return 
			Matrix4x4<T>({ 1, 0, 0,  aPoint.x, 0, 1, 0,  aPoint.y, 0, 0, 1,  aPoint.z, 0, 0, 0, 1 }) *
			CreateRotationAroundY(anAngle) * 
			Matrix4x4<T>({ 1, 0, 0, -aPoint.x, 0, 1, 0, -aPoint.y, 0, 0, 1, -aPoint.z, 0, 0, 0, 1 });
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointZ(const T anAngle, const Vector4<T> &aPoint)
	{
		return 
			Matrix4x4<T>({ 1, 0, 0,  aPoint.x, 0, 1, 0,  aPoint.y, 0, 0, 1,  aPoint.z, 0, 0, 0, 1 }) *
			CreateRotationAroundZ(anAngle) * 
			Matrix4x4<T>({ 1, 0, 0, -aPoint.x, 0, 1, 0, -aPoint.y, 0, 0, 1, -aPoint.z, 0, 0, 0, 1 });
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationFromDirection(const Vector3<T>& aDirection)
	{
		Vector3<T> forward = aDirection.GetNormalized();
		Vector3<T> right = forward.Cross(-Vector3<T>(0, 1, 0)).GetNormalized();
		Vector3<T> up = -right.Cross(forward);

		return { right.x,	right.y,	right.z,	0,
				up.x,		up.y,		up.z,		0,
				forward.x,	forward.y,	forward.z,	0,
				0,			0,			0,			1 };
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Identity()
	{
		return Matrix4x4<T>();
	}


	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T> &aMatrixToTranspose)
	{
		Matrix4x4<T> transpose;

		for (size_t row = 0; row < 4; ++row)
		{
			for (size_t column = 0; column < 4; ++column)
			{
				transpose.myData[column][row] = aMatrixToTranspose.myData[row][column];
			}
		}

		return transpose;
	}

	template<class T>
	inline  Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T> &aTransform)
	{
		const T* elem = aTransform.myElements;

		return { elem[0], elem[4], elem[8], elem[3],
				elem[1], elem[5], elem[9], elem[7],
				elem[2], elem[6], elem[10], elem[11],
				
				((-elem[12]) * elem[0]) + ((-elem[13]) * elem[1]) + ((-elem[14]) * elem[2]),
				((-elem[12]) * elem[4]) + ((-elem[13]) * elem[5]) + ((-elem[14]) * elem[6]),
				((-elem[12]) * elem[8]) + ((-elem[13]) * elem[9]) + ((-elem[14]) * elem[10]), 
				elem[15] };
	}

	template<class T>
	inline  Matrix4x4<T> Matrix4x4<T>::GetRealInverse(const Matrix4x4<T>& aTransform)
	{
		auto m = aTransform;
		T A2323 = m(3,3) * m(4,4) - m(3,4) * m(4,3);
		T A1323 = m(3,2) * m(4,4) - m(3,4) * m(4,2);
		T A1223 = m(3,2) * m(4,3) - m(3,3) * m(4,2);
		T A0323 = m(3,1) * m(4,4) - m(3,4) * m(4,1);
		T A0223 = m(3,1) * m(4,3) - m(3,3) * m(4,1);
		T A0123 = m(3,1) * m(4,2) - m(3,2) * m(4,1);
		T A2313 = m(2,3) * m(4,4) - m(2,4) * m(4,3);
		T A1313 = m(2,2) * m(4,4) - m(2,4) * m(4,2);
		T A1213 = m(2,2) * m(4,3) - m(2,3) * m(4,2);
		T A2312 = m(2,3) * m(3,4) - m(2,4) * m(3,3);
		T A1312 = m(2,2) * m(3,4) - m(2,4) * m(3,2);
		T A1212 = m(2,2) * m(3,3) - m(2,3) * m(3,2);
		T A0313 = m(2,1) * m(4,4) - m(2,4) * m(4,1);
		T A0213 = m(2,1) * m(4,3) - m(2,3) * m(4,1);
		T A0312 = m(2,1) * m(3,4) - m(2,4) * m(3,1);
		T A0212 = m(2,1) * m(3,3) - m(2,3) * m(3,1);
		T A0113 = m(2,1) * m(4,2) - m(2,2) * m(4,1);
		T A0112 = m(2,1) * m(3,2) - m(2,2) * m(3,1);

		T det = m(1,1) * (m(2,2) * A2323 - m(2,3) * A1323 + m(2,4) * A1223)
			- m(1,2) * (m(2,1) * A2323 - m(2,3) * A0323 + m(2,4) * A0223)
			+ m(1,3) * (m(2,1) * A1323 - m(2,2) * A0323 + m(2,4) * A0123)
			- m(1,4) * (m(2,1) * A1223 - m(2,2) * A0223 + m(2,3) * A0123);
		det = 1.0f / det;


		return {	det *  (m(2, 2) * A2323 - m(2, 3) * A1323 + m(2, 4) * A1223),
					det * -(m(1, 2) * A2323 - m(1, 3) * A1323 + m(1, 4) * A1223),
					det *  (m(1, 2) * A2313 - m(1, 3) * A1313 + m(1, 4) * A1213),
					det * -(m(1, 2) * A2312 - m(1, 3) * A1312 + m(1, 4) * A1212),
					det * -(m(2, 1) * A2323 - m(2, 3) * A0323 + m(2, 4) * A0223),
					det *  (m(1, 1) * A2323 - m(1, 3) * A0323 + m(1, 4) * A0223),
					det * -(m(1, 1) * A2313 - m(1, 3) * A0313 + m(1, 4) * A0213),
					det *  (m(1, 1) * A2312 - m(1, 3) * A0312 + m(1, 4) * A0212),
					det *  (m(2, 1) * A1323 - m(2, 2) * A0323 + m(2, 4) * A0123),
					det * -(m(1, 1) * A1323 - m(1, 2) * A0323 + m(1, 4) * A0123),
					det *  (m(1, 1) * A1313 - m(1, 2) * A0313 + m(1, 4) * A0113),
					det * -(m(1, 1) * A1312 - m(1, 2) * A0312 + m(1, 4) * A0112),
					det * -(m(2, 1) * A1223 - m(2, 2) * A0223 + m(2, 3) * A0123),
					det *  (m(1, 1) * A1223 - m(1, 2) * A0223 + m(1, 3) * A0123),
					det * -(m(1, 1) * A1213 - m(1, 2) * A0213 + m(1, 3) * A0113),
					det *  (m(1, 1) * A1212 - m(1, 2) * A0212 + m(1, 3) * A0112) };
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundX(const T anAngle)
	{
		*this *= CreateRotationAroundX(anAngle);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundY(const T anAngle)
	{
		(*this) *= CreateRotationAroundY(anAngle);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundZ(const T anAngle)
	{
		(*this) *= CreateRotationAroundZ(anAngle);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointX(const T anAngle, const Vector4<T>& aPoint)
	{
		(*this) *= CreateRotationAroundPointX(anAngle, aPoint);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointY(const T anAngle, const Vector4<T>& aPoint)
	{
		(*this) *= CreateRotationAroundPointY(anAngle, aPoint);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointZ(const T anAngle, const Vector4<T>& aPoint)
	{
		(*this) *= CreateRotationAroundPointZ(anAngle, aPoint);
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		for (size_t index = 0; index < sizeof(myElements) / sizeof(T); ++index)
		{
			myElements[index] = aMatrix.myElements[index];
		}

		return (*this);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> copy(*this);
		copy += aMatrix;
		return copy;
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		for (size_t i = 0; i < sizeof(myElements) / sizeof(T); i++)
		{
			myElements[i] += aMatrix.myElements[i];
		}
		return *this;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> copy(*this);
		copy -= aMatrix;
		return copy;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		for (size_t i = 0; i < sizeof(myElements) / sizeof(T); i++)
		{
			myElements[i] -= aMatrix.myElements[i];
		}
		return *this;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> out(*this);
		out *= aMatrix;
		return out;
	}

	template<class T>
	inline Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		return Vector4<T>(	aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1),
							aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2),
							aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3),
							aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4));
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		T data[16];
		memcpy(data, myElements, 16 * sizeof(float));

		for (size_t i = 0; i < 16; i++)
		{
			myElements[i] = 0;
			size_t x = i % 4;
			size_t y = i / 4;

			for (size_t ii = 0; ii < 4; ii++)
			{
				myElements[i] += data[y * 4 + ii] * aMatrix.myElements[ii * 4 + x];
			}
		}

		return *this;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const T aScalar) const
	{
		Matrix4x4<T> out(*this);
		out *= aScalar;
		return out;
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const T aScalar)
	{
		for (size_t i = 0; i < sizeof(myElements) / sizeof(T); i++)
		{
			myElements[i] *= aScalar;
		}
		return (*this);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator/(const T aScalar) const
	{
		return operator *(1 / aScalar);
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator/=(const T aScalar)
	{
		return operator *=(1/aScalar);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(void)
	{
		return Matrix4x4<T>(-myElements[0], -myElements[1], -myElements[2], -myElements[3],
			-myElements[4], -myElements[5], -myElements[6], -myElements[7],
			-myElements[8], -myElements[9], -myElements[10], -myElements[11],
			-myElements[12], -myElements[13], -myElements[14], -myElements[15]);
	}

	template<class T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aMatrix) const
	{
		for (size_t i = 0; i < 16; i++)
		{
			if (myElements[i] != aMatrix.myElements[i])
			{
				return false;
			}
		}
		return true;
	}

	template<>
	inline bool Matrix4x4<float>::operator==(const Matrix4x4<float>& aMatrix) const
	{
		const float eps = 1e-50;
		for (size_t i = 0; i < 16; i++)
		{
			if (abs(myElements[i] - aMatrix.myElements[i]) > eps)
			{
				return false;
			}
		}
		return true;
	}

	template<>
	inline bool Matrix4x4<double>::operator==(const Matrix4x4<double>& aMatrix) const
	{
		const double eps = 1e-50;
		for (size_t i = 0; i < 16; i++)
		{
			if (abs(myElements[i] - aMatrix.myElements[i]) > eps)
			{
				return false;
			}
		}
		return true;
	}

	template<class T>
	inline bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& aMatrix) const
	{
		return !((*this) == aMatrix);
	}

	template<class T>
	inline T& Matrix4x4<T>::operator()(const unsigned int aRow, const unsigned int aColumn)
	{
		if (aRow > 0 && aRow <= 4 && aColumn > 0 && aColumn <= 4)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix4x4 index out of range.", "");
		return myData[0][0];
	}

	template<class T>
	inline const T& Matrix4x4<T>::operator()(const unsigned int aRow, const unsigned int aColumn) const
	{
		if (aRow > 0 && aRow <= 4 && aColumn > 0 && aColumn <= 4)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix4x4 index out of range.", "");
		return myData[0][0];
	}

	template<class T>
	inline T& Matrix4x4<T>::operator[](const unsigned int anIndex)
	{
		if (anIndex < 0 || anIndex >= sizeof(myElements) / sizeof(T))
		{
			SYSERROR("Matrix4x4 index out of range.", "");
		}
		return myElements[anIndex];
	}

	template<class T>
	inline const T& Matrix4x4<T>::operator[](const unsigned int anIndex) const
	{
		if (anIndex < 0 || anIndex >= sizeof(myElements) / sizeof(T))
		{
			SYSERROR("Matrix4x4 index out of range.", "");
		}
		return myElements[anIndex];
	}
}

typedef CommonUtilities::Matrix4x4<float> M44f;
typedef CommonUtilities::Matrix4x4<double> M44d;
typedef CommonUtilities::Matrix4x4<int> M44i;