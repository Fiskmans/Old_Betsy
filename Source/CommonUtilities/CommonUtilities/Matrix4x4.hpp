#pragma once
#include "../../Tools/Logger.h"

#define M44F CommonUtilities::Matrix4x4<float>

#define DATASIZE 4
#define ELEMENTSIZE 16

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
		Matrix4x4(const T aXX, const T aXY, const T aXZ, const T aXW,
			const T aYX, const T aYY, const T aYZ, const T aYW,
			const T aZX, const T aZY, const T aZZ, const T aZW,
			const T aWX, const T aWY, const T aWZ, const T aWW);
		Matrix4x4(const T(&anArrayMatrix)[4][4]);
		Matrix4x4(const T(&anArrayMatrix)[16]);
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
		Matrix4x4<T> operator+=(const Matrix4x4<T> &aMatrix);

		Matrix4x4<T> operator-(const Matrix4x4<T> &aMatrix) const;
		Matrix4x4<T> operator-=(const Matrix4x4<T> &aMatrix);

		Matrix4x4<T> operator*(const Matrix4x4<T> &aMatrix) const;
		Matrix4x4<T> operator*=(const Matrix4x4<T> &aMatrix);

		Matrix4x4<T> operator*(const T aScalar) const;
		Matrix4x4<T> operator*=(const T aScalar);

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

		const T(&GetElements())[16];
		const T(&GetElements() const)[16];

	private:

		union
		{
			T myData[DATASIZE][DATASIZE];
			T myElements[ELEMENTSIZE];
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
		for (size_t index = 0; index < ELEMENTSIZE; index++)
		{
			myElements[index] = aMatrix.myElements[index];
		}
	}


	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const T aXX, const T aXY, const T aXZ, const T aXW, const T aYX, const T aYY, const T aYZ, const T aYW, const T aZX, const T aZY, const T aZZ, const T aZW, const T aWX, const T aWY, const T aWZ, const T aWW)
		: myElements{ aXX, aXY, aXZ, aXW, aYX, aYY, aYZ, aYW, aZX, aZY, aZZ, aZW, aWX, aWY, aWZ, aWW }
	{
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const T(&anArrayMatrix)[4][4]) : myElements{}
	{
		for (size_t row = 0; row < DATASIZE; ++row)
		{
			for (size_t column = 0; column < DATASIZE; ++column)
			{
				myData[row][column] = anArrayMatrix[row][column];
			}
		}
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const T(&anArrayMatrix)[16]) : myElements{}
	{
		for (size_t row = 0; row < DATASIZE; ++row)
		{
			for (size_t column = 0; column < DATASIZE; ++column)
			{
				myData[row][column] = anArrayMatrix[row * DATASIZE + column];
			}
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
		if (anInitList.size() > ELEMENTSIZE)
		{
			SYSERROR("Initializer list for Matrix4x4 too big!","");
		}

		for (size_t i = 0; i < ELEMENTSIZE; ++i)
		{
			myElements[i] = i < anInitList.size() ? *(anInitList.begin() + i) : T();
		}
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix4x4<T>(1, 0, 0, 0,
							0, c, s, 0,
							0, -s, c, 0,
							0, 0, 0, 1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix4x4<T>(c, 0, -s, 0,
							0, 1, 0, 0,
							s, 0, c, 0,
							0, 0, 0, 1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix4x4<T>(c, s, 0, 0,
							-s, c, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointX(const T anAngle, const Vector4<T> &aPoint)
	{
		return Matrix4x4<T>(Matrix4x4<T>(1, 0, 0, aPoint.x, 0, 1, 0, aPoint.y, 0, 0, 1, aPoint.z, 0, 0, 0, 1) *
			CreateRotationAroundX(anAngle) * Matrix4x4<T>(1, 0, 0, -aPoint.x, 0, 1, 0, -aPoint.y, 0, 0, 1, -aPoint.z, 0, 0, 0, 1));
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointY(const T anAngle, const Vector4<T> &aPoint)
	{
		return Matrix4x4<T>(Matrix4x4<T>(1, 0, 0, aPoint.x, 0, 1, 0, aPoint.y, 0, 0, 1, aPoint.z, 0, 0, 0, 1) *
			CreateRotationAroundY(anAngle) * Matrix4x4<T>(1, 0, 0, -aPoint.x, 0, 1, 0, -aPoint.y, 0, 0, 1, -aPoint.z, 0, 0, 0, 1));
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointZ(const T anAngle, const Vector4<T> &aPoint)
	{
		return Matrix4x4<T>(Matrix4x4<T>(1, 0, 0, aPoint.x, 0, 1, 0, aPoint.y, 0, 0, 1, aPoint.z, 0, 0, 0, 1) *
			CreateRotationAroundZ(anAngle) * Matrix4x4<T>(1, 0, 0, -aPoint.x, 0, 1, 0, -aPoint.y, 0, 0, 1, -aPoint.z, 0, 0, 0, 1));
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationFromDirection(const Vector3<T>& aDirection)
	{
		Vector3<T> forward = aDirection.GetNormalized();
		Vector3<T> right = forward.Cross(-Vector3<T>(0, 1, 0)).GetNormalized();
		Vector3<T> up = -right.Cross(forward);

		return Matrix4x4<T>(right.x,	right.y,	right.z,	0,
							up.x,		up.y,		up.z,		0,
							forward.x,	forward.y,	forward.z,	0,
							0,			0,			0,			1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Identity()
	{
		return Matrix4x4<T>();
	}


	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T> &aMatrixToTranspose)
	{
		static T transpose[DATASIZE][DATASIZE];

		for (size_t row = 0; row < DATASIZE; ++row)
		{
			for (size_t column = 0; column < DATASIZE; ++column)
			{
				transpose[column][row] = aMatrixToTranspose.myData[row][column];
			}
		}

		return Matrix4x4<T>(transpose);
	}

	template<class T>
	inline  Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T> &aTransform)
	{
		return Matrix4x4<T>(aTransform.myElements[0], aTransform.myElements[4], aTransform.myElements[8], aTransform.myElements[3],
							aTransform.myElements[1], aTransform.myElements[5], aTransform.myElements[9], aTransform.myElements[7],
							aTransform.myElements[2], aTransform.myElements[6], aTransform.myElements[10], aTransform.myElements[11],
							((-aTransform.myElements[12]) * aTransform.myElements[0]) + ((-aTransform.myElements[13]) * aTransform.myElements[1]) + ((-aTransform.myElements[14]) * aTransform.myElements[2]),
							((-aTransform.myElements[12]) * aTransform.myElements[4]) + ((-aTransform.myElements[13]) * aTransform.myElements[5]) + ((-aTransform.myElements[14]) * aTransform.myElements[6]),
							((-aTransform.myElements[12]) * aTransform.myElements[8]) + ((-aTransform.myElements[13]) * aTransform.myElements[9]) + ((-aTransform.myElements[14]) * aTransform.myElements[10]), aTransform.myElements[15]);
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


		return Matrix4x4<T>(det * (m(2, 2) * A2323 - m(2, 3) * A1323 + m(2, 4) * A1223),
			det * -(m(1, 2) * A2323 - m(1, 3) * A1323 + m(1, 4) * A1223),
			det * (m(1, 2) * A2313 - m(1, 3) * A1313 + m(1, 4) * A1213),
			det * -(m(1, 2) * A2312 - m(1, 3) * A1312 + m(1, 4) * A1212),
			det * -(m(2, 1) * A2323 - m(2, 3) * A0323 + m(2, 4) * A0223),
			det * (m(1, 1) * A2323 - m(1, 3) * A0323 + m(1, 4) * A0223),
			det * -(m(1, 1) * A2313 - m(1, 3) * A0313 + m(1, 4) * A0213),
			det * (m(1, 1) * A2312 - m(1, 3) * A0312 + m(1, 4) * A0212),
			det * (m(2, 1) * A1323 - m(2, 2) * A0323 + m(2, 4) * A0123),
			det * -(m(1, 1) * A1323 - m(1, 2) * A0323 + m(1, 4) * A0123),
			det * (m(1, 1) * A1313 - m(1, 2) * A0313 + m(1, 4) * A0113),
			det * -(m(1, 1) * A1312 - m(1, 2) * A0312 + m(1, 4) * A0112),
			det * -(m(2, 1) * A1223 - m(2, 2) * A0223 + m(2, 3) * A0123),
			det * (m(1, 1) * A1223 - m(1, 2) * A0223 + m(1, 3) * A0123),
			det * -(m(1, 1) * A1213 - m(1, 2) * A0213 + m(1, 3) * A0113),
			det * (m(1, 1) * A1212 - m(1, 2) * A0212 + m(1, 3) * A0112));
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundX(const T anAngle)
	{
		(*this) = CreateRotationAroundX(anAngle) * (*this);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundY(const T anAngle)
	{
		(*this) = CreateRotationAroundY(anAngle) * (*this);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundZ(const T anAngle)
	{
		(*this) = CreateRotationAroundZ(anAngle)* (*this);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointX(const T anAngle, const Vector4<T>& aPoint)
	{
		(*this) = CreateRotationAroundPointX(anAngle, aPoint) * (*this);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointY(const T anAngle, const Vector4<T>& aPoint)
	{
		(*this) = CreateRotationAroundPointY(anAngle, aPoint) * (*this);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointZ(const T anAngle, const Vector4<T>& aPoint)
	{
		(*this) = CreateRotationAroundPointZ(anAngle, aPoint) * (*this);
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		for (size_t index = 0; index < ELEMENTSIZE; ++index)
		{
			myElements[index] = aMatrix.myElements[index];
		}

		return (*this);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix) const
	{
		return Matrix4x4<T>(myElements[0] + aMatrix.myElements[0], myElements[1] + aMatrix.myElements[1], myElements[2] + aMatrix.myElements[2], myElements[3] + aMatrix.myElements[3], 
							myElements[4] + aMatrix.myElements[4], myElements[5] + aMatrix.myElements[5], myElements[6] + aMatrix.myElements[6], myElements[7] + aMatrix.myElements[7], 
							myElements[8] + aMatrix.myElements[8], myElements[9] + aMatrix.myElements[9], myElements[10] + aMatrix.myElements[10], myElements[11] + aMatrix.myElements[11],
							myElements[12] + aMatrix.myElements[12], myElements[13] + aMatrix.myElements[13], myElements[14] + aMatrix.myElements[14], myElements[15] + aMatrix.myElements[15]);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		return (*this) = (*this) + aMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix) const
	{
		return Matrix4x4<T>(myElements[0] - aMatrix.myElements[0], myElements[1] - aMatrix.myElements[1], myElements[2] - aMatrix.myElements[2], myElements[3] - aMatrix.myElements[3],
							myElements[4] - aMatrix.myElements[4], myElements[5] - aMatrix.myElements[5], myElements[6] - aMatrix.myElements[6], myElements[7] - aMatrix.myElements[7],
							myElements[8] - aMatrix.myElements[8], myElements[9] - aMatrix.myElements[9], myElements[10] - aMatrix.myElements[10], myElements[11] - aMatrix.myElements[11],
							myElements[12] - aMatrix.myElements[12], myElements[13] - aMatrix.myElements[13], myElements[14] - aMatrix.myElements[14], myElements[15] - aMatrix.myElements[15]);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		return (*this) = (*this) - aMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix) const
	{
		return Matrix4x4<T>(myData[0][0] * aMatrix.myData[0][0] + myData[0][1] * aMatrix.myData[1][0] + myData[0][2] * aMatrix.myData[2][0] + myData[0][3] * aMatrix.myData[3][0],
							myData[0][0] * aMatrix.myData[0][1] + myData[0][1] * aMatrix.myData[1][1] + myData[0][2] * aMatrix.myData[2][1] + myData[0][3] * aMatrix.myData[3][1],
							myData[0][0] * aMatrix.myData[0][2] + myData[0][1] * aMatrix.myData[1][2] + myData[0][2] * aMatrix.myData[2][2] + myData[0][3] * aMatrix.myData[3][2],
							myData[0][0] * aMatrix.myData[0][3] + myData[0][1] * aMatrix.myData[1][3] + myData[0][2] * aMatrix.myData[2][3] + myData[0][3] * aMatrix.myData[3][3],

							myData[1][0] * aMatrix.myData[0][0] + myData[1][1] * aMatrix.myData[1][0] + myData[1][2] * aMatrix.myData[2][0] + myData[1][3] * aMatrix.myData[3][0],
							myData[1][0] * aMatrix.myData[0][1] + myData[1][1] * aMatrix.myData[1][1] + myData[1][2] * aMatrix.myData[2][1] + myData[1][3] * aMatrix.myData[3][1],
							myData[1][0] * aMatrix.myData[0][2] + myData[1][1] * aMatrix.myData[1][2] + myData[1][2] * aMatrix.myData[2][2] + myData[1][3] * aMatrix.myData[3][2],
							myData[1][0] * aMatrix.myData[0][3] + myData[1][1] * aMatrix.myData[1][3] + myData[1][2] * aMatrix.myData[2][3] + myData[1][3] * aMatrix.myData[3][3],

							myData[2][0] * aMatrix.myData[0][0] + myData[2][1] * aMatrix.myData[1][0] + myData[2][2] * aMatrix.myData[2][0] + myData[2][3] * aMatrix.myData[3][0],
							myData[2][0] * aMatrix.myData[0][1] + myData[2][1] * aMatrix.myData[1][1] + myData[2][2] * aMatrix.myData[2][1] + myData[2][3] * aMatrix.myData[3][1],
							myData[2][0] * aMatrix.myData[0][2] + myData[2][1] * aMatrix.myData[1][2] + myData[2][2] * aMatrix.myData[2][2] + myData[2][3] * aMatrix.myData[3][2],
							myData[2][0] * aMatrix.myData[0][3] + myData[2][1] * aMatrix.myData[1][3] + myData[2][2] * aMatrix.myData[2][3] + myData[2][3] * aMatrix.myData[3][3],

							myData[3][0] * aMatrix.myData[0][0] + myData[3][1] * aMatrix.myData[1][0] + myData[3][2] * aMatrix.myData[2][0] + myData[3][3] * aMatrix.myData[3][0],
							myData[3][0] * aMatrix.myData[0][1] + myData[3][1] * aMatrix.myData[1][1] + myData[3][2] * aMatrix.myData[2][1] + myData[3][3] * aMatrix.myData[3][1],
							myData[3][0] * aMatrix.myData[0][2] + myData[3][1] * aMatrix.myData[1][2] + myData[3][2] * aMatrix.myData[2][2] + myData[3][3] * aMatrix.myData[3][2],
							myData[3][0] * aMatrix.myData[0][3] + myData[3][1] * aMatrix.myData[1][3] + myData[3][2] * aMatrix.myData[2][3] + myData[3][3] * aMatrix.myData[3][3]);
	}

	template<class T>
	inline Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		return Vector4<T>(aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1), 
						  aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2),
						  aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3),
						  aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4));
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		return (*this) = (*this) * aMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const T aScalar) const
	{
		return Matrix4x4<T>(myElements[0] * aScalar, myElements[1] * aScalar, myElements[2] * aScalar, myElements[3] * aScalar,
							myElements[4] * aScalar, myElements[5] * aScalar, myElements[6] * aScalar, myElements[7] * aScalar,
							myElements[8] * aScalar, myElements[9] * aScalar, myElements[10] * aScalar, myElements[11] * aScalar,
							myElements[12] * aScalar, myElements[13] * aScalar, myElements[14] * aScalar, myElements[15] * aScalar);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*=(const T aScalar)
	{
		return (*this) = (*this) * aScalar;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(void)
	{
		return Matrix4x4<T>(-myElements[0],  -myElements[1],  -myElements[2],  -myElements[3],
							-myElements[4],  -myElements[5],  -myElements[6],  -myElements[7], 
							-myElements[8],  -myElements[9],  -myElements[10], -myElements[11],
							-myElements[12], -myElements[13], -myElements[14], -myElements[15]);
	}

	template<class T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aMatrix) const
	{
		return  (myElements[0] == aMatrix.myElements[0] &&
				myElements[1] == aMatrix.myElements[1] &&
				myElements[2] == aMatrix.myElements[2] &&
				myElements[3] == aMatrix.myElements[3] &&
				myElements[4] == aMatrix.myElements[4] &&
				myElements[5] == aMatrix.myElements[5] &&
				myElements[6] == aMatrix.myElements[6] &&
				myElements[7] == aMatrix.myElements[7] &&
				myElements[8] == aMatrix.myElements[8] &&
				myElements[9] == aMatrix.myElements[9] &&
				myElements[10] == aMatrix.myElements[10] &&
				myElements[11] == aMatrix.myElements[11] &&
				myElements[12] == aMatrix.myElements[12] &&
				myElements[13] == aMatrix.myElements[13] &&
				myElements[14] == aMatrix.myElements[14] &&
				myElements[15] == aMatrix.myElements[15]);
	}

	template<class T>
	inline bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& aMatrix) const
	{
		return !((*this) == aMatrix);
	}

	template<class T>
	inline T& Matrix4x4<T>::operator()(const unsigned int aRow, const unsigned int aColumn)
	{
		if (aRow > 0 && aRow <= DATASIZE && aColumn > 0 && aColumn <= DATASIZE)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix4x4 index out of range.","");
		return myData[0][0];
	}

	template<class T>
	inline const T& Matrix4x4<T>::operator()(const unsigned int aRow, const unsigned int aColumn) const
	{
		if (aRow > 0 && aRow <= DATASIZE && aColumn > 0 && aColumn <= DATASIZE)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix4x4 index out of range.","");
		return myData[0][0];
	}

	template<class T>
	inline T& Matrix4x4<T>::operator[](const unsigned int anIndex)
	{
		if (anIndex < 0 || anIndex >= ELEMENTSIZE)
		{
			SYSERROR("Matrix4x4 index out of range.","");
		}
		return myElements[anIndex];
	}

	template<class T>
	inline const T& Matrix4x4<T>::operator[](const unsigned int anIndex) const
	{
		if (anIndex < 0 || anIndex >= ELEMENTSIZE)
		{
			SYSERROR("Matrix4x4 index out of range.","");
		}
		return myElements[anIndex];
	}

	template<class T>
	inline const T(&Matrix4x4<T>::GetElements())[16]
	{
		return myElements;
	}

	template<class T>
	inline const T(&Matrix4x4<T>::GetElements() const)[16]
	{
		return myElements;
	}
}

#undef DATASIZE 
#undef ELEMENTSIZE 
