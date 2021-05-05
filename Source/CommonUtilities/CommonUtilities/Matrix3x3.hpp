#pragma once
#include "../../Tools/Logger.h"

#define CU CommonUtilities
#define M33F CommonUtilities::Matrix3x3<float>;

#define	DATASIZE 3
#define ELEMENTSIZE 9

namespace CommonUtilities
{
	template <class T>
	class Vector3;

	template <class T>
	class Matrix4x4;

	template <class T>
	class Matrix3x3
	{
	public:
		Matrix3x3(); //Creates an identity matrix
		Matrix3x3(const Matrix3x3<T> &aMatrix);
		Matrix3x3(const Matrix4x4<T> &aMatrix);
		Matrix3x3(const T aXX, const T aXY, const T aXZ,
			const T aYX, const T aYY, const T aYZ,
			const T aZX, const T aZY, const T aZZ);
		Matrix3x3(const T (&anArrayMatrix)[3][3]);
		Matrix3x3(const T (&anArrayMatrix)[9]);
		Matrix3x3(const std::initializer_list<T>& anInitList);
		~Matrix3x3() = default;

		static Matrix3x3<T> CreateRotationAroundX(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundY(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundZ(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundPointX(const Vector3<T> &aPoint, const T anAngle); 
		static Matrix3x3<T> CreateRotationAroundPointY(const Vector3<T> &aPoint, const T anAngle); 
		//static Matrix3x3<T> CreateRotationAroundPointZ(const Vector3<T> &aPoint, const T anAngle); 

		static Matrix3x3<T> CreateRotationFromDirection(const Vector3<T>& aDirection);

		static Matrix3x3<T> CreateMysteryRotationAroundPoint(const Vector3<T> &aPoint, const T anAngle); //Old func that I'm not 100% sure what it does (it's still here so it can be tested)

		static Matrix3x3<T> Identity();
	
		void RotateAroundX(const T anAngle);
		void RotateAroundY(const T anAngle);
		void RotateAroundZ(const T anAngle);
		void RotateAroundPointX(const Vector3<T> &aPoint, const T anAngle);
		void RotateAroundPointY(const Vector3<T> &aPoint, const T anAngle);
		//void RotateAroundPointZ(const Vector3<T> &aPoint, const T anAngle);

		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);
	
		//Operators
		Matrix3x3<T> &operator=(const Matrix3x3<T>& aMatrix);

		Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator+=(const Matrix3x3<T>& aMatrix);
	
		Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator-=(const Matrix3x3<T>& aMatrix);
	
		Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator*=(const Matrix3x3<T>& aMatrix);
	
		Matrix3x3<T> operator*(const T aScalar) const;
		Matrix3x3<T> operator*=(const T aScalar);

		Matrix3x3<T> operator-(void);
	
		//Vector3<T> &operator*(const Vector3<T> &aVector, const Matrix3x3<T> &aMatrix) //has to be non-member

		bool operator==(const Matrix3x3<T> &aMatrix) const;
		bool operator!=(const Matrix3x3<T> &aMatrix) const;

		T& operator()(const unsigned int aRow, const unsigned int aColumn);
		const T& operator()(const unsigned int aRow, const unsigned int aColumn) const;

		T& operator[](const unsigned int anIndex);
		const T& operator[](const unsigned int anIndex) const;

		const T(&GetElements())[9];

	private:

		union 
		{
			T myData[DATASIZE][DATASIZE];
			T myElements[ELEMENTSIZE];
		};

	};

	//*****************************************************************************

	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
		:myElements{ 1,0,0, 0,1,0, 0,0,1 }
	{
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T> &aMatrix) : myElements{}
	{
		for (size_t index = 0; index < ELEMENTSIZE; index++)
		{
			myElements[index] = aMatrix.myElements[index];
		}
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix) : myElements{}
	{
		for (unsigned int row = 0; row < DATASIZE; ++row)
		{
			for (unsigned int column = 0; column < DATASIZE; ++column)
			{
				myData[row][column] = aMatrix(row + 1, column + 1);
			}
		}
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const T aXX, const T aXY, const T aXZ, const T aYX, const T aYY, const T aYZ, const T aZX, const T aZY, const T aZZ)
		: myElements{ aXX, aXY, aXZ, aYX, aYY, aYZ, aZX, aZY, aZZ }
	{
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const T (&anArrayMatrix)[3][3]) : myElements{}
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
	inline Matrix3x3<T>::Matrix3x3(const T(&anArrayMatrix)[9]) : myElements{}
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
	inline Matrix3x3<T>::Matrix3x3(const std::initializer_list<T>& anInitList) : myElements{}
	{
		if (anInitList.size() > ELEMENTSIZE)
		{
			SYSERROR("Initializer list for Matrix3x3 too big!","");
		}

		for (size_t i = 0; i < ELEMENTSIZE; ++i)
		{
			myElements[i] = i < anInitList.size() ? *(anInitList.begin() + i) : T();
		}
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix3x3<T>(1, 0, 0, 
							0, c, s,
							0, -s, c);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix3x3<T>(c, 0, -s,
							0, 1, 0, 
							s, 0, c);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix3x3<T>(c, s, 0, 
							-s, c, 0, 
							0, 0, 1);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundPointX(const Vector3<T>& aPoint, const T anAngle)
	{
		return Matrix3x3<T>(Matrix3x3<T>(1, 0, aPoint.x, 0, 1, aPoint.y, 0, 0, 1) *
			CreateRotationAroundX(anAngle) * Matrix3x3<T>(1, 0, -aPoint.x, 0, 1, -aPoint.y, 0, 0, -aPoint.z));
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundPointY(const Vector3<T>& aPoint, const T anAngle)
	{
		return Matrix3x3<T>(Matrix3x3<T>(1, 0, aPoint.x, 0, 1, aPoint.y, 0, 0, 1) *
			CreateRotationAroundY(anAngle) * Matrix3x3<T>(1, 0, -aPoint.x, 0, 1, -aPoint.y, 0, 0, 1));
	}

	//template<class T>
	//inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundPointZ(const Vector3<T>& aPoint, const T anAngle)
	//{
	//	return Matrix3x3<T>(Matrix3x3<T>(1, 0, aPoint.x, 0, 1, aPoint.y, 0, 0, aPoint.z) *
	//		CreateRotationAroundY(anAngle) * Matrix3x3<T>(1, 0, -aPoint.x, 0, 1, -aPoint.y, 0, 0, 1));
	//}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateMysteryRotationAroundPoint(const Vector3<T>& aPoint, const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		T rotated[3][3];

		rotated[0] = { aPoint.x * aPoint.x * (1 - c) + c, aPoint.x * aPoint.y * (1 - c) - (aPoint.z * s), aPoint.x * aPoint.z * (1 - c) + (aPoint.y * s) };
		rotated[1] = { aPoint.x * aPoint.y * (1 - c) + (aPoint.z * s), aPoint.y * aPoint.y * (1 - c) + c, aPoint.y * aPoint.z * (1 - c) - (aPoint.x * s) };
		rotated[2] = { aPoint.x * aPoint.z * (1 - c) - (aPoint.y * s), aPoint.y * aPoint.z * (1 - c) + (aPoint.x * s), aPoint.z * aPoint.z * (1 - c) + c };

		return Matrix3x3<T>(rotated);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationFromDirection(const Vector3<T>& aDirection)
	{
		Vector3<T> forward = aDirection.GetNormalized();
		Vector3<T> right = forward.Cross(-Vector3<T>(0, 1, 0)).GetNormalized();
		Vector3<T> up = -right.Cross(forward);

		return Matrix3x3<T>(right.x,	right.y,	right.z,
							up.x,		up.y,		up.z, 
							forward.x,	forward.y,	forward.z);
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Identity()
	{
		return Matrix3x3<T>();
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		T transpose[DATASIZE][DATASIZE];

		for (size_t row = 0; row < DATASIZE; ++row)
		{
			for (size_t column = 0; column < DATASIZE; ++column)
			{
				transpose[column][row] = aMatrixToTranspose.myData[row][column];
			}
		}

		return Matrix3x3<T>(transpose);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundX(const T anAngle)
	{
		(*this) = CreateRotationAroundX(anAngle) * (*this);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundY(const T anAngle)
	{
		(*this) = CreateRotationAroundY(anAngle) * (*this);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundZ(const T anAngle)
	{
		(*this) = CreateRotationAroundZ(anAngle) * (*this);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundPointX(const Vector3<T>& aPoint, const T anAngle)
	{ 
		(*this) = CreateRotationAroundPointX(aPoint, anAngle) * (*this);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundPointY(const Vector3<T>& aPoint, const T anAngle)
	{
		(*this) = CreateRotationAroundPointY(aPoint, anAngle) * (*this);
	}

	//template<class T>
	//inline void Matrix3x3<T>::RotateAroundPointZ(const Vector3<T>& aPoint, const T anAngle)
	//{
	//	(*this) = CreateRotationAroundPointZ(aPoint, anAngle) * (*this);
	//}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		for (size_t index = 0; index < ELEMENTSIZE; ++index)
		{
			myElements[index] = aMatrix.myElements[index];
		}

		return (*this);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix) const
	{
		return Matrix3x3<T>(myElements[0] + aMatrix.myElements[0], myElements[1] + aMatrix.myElements[1], myElements[2] + aMatrix.myElements[2],
							myElements[3] + aMatrix.myElements[3], myElements[4] + aMatrix.myElements[4], myElements[5] + aMatrix.myElements[5], 
							myElements[6] + aMatrix.myElements[6], myElements[7] + aMatrix.myElements[7], myElements[8] + aMatrix.myElements[8]);	
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		return (*this) = (*this) + aMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix) const
	{
		return Matrix3x3<T>(myElements[0] - aMatrix.myElements[0], myElements[1] - aMatrix.myElements[1], myElements[2] - aMatrix.myElements[2],
							myElements[3] - aMatrix.myElements[3], myElements[4] - aMatrix.myElements[4], myElements[5] - aMatrix.myElements[5],
							myElements[6] - aMatrix.myElements[6], myElements[7] - aMatrix.myElements[7], myElements[8] - aMatrix.myElements[8]);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		return (*this) = (*this) - aMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix) const
	{
		return Matrix3x3<T>(myData[0][0] * aMatrix.myData[0][0] + myData[0][1] * aMatrix.myData[1][0] + myData[0][2] * aMatrix.myData[2][0], 
							myData[0][0] * aMatrix.myData[0][1] + myData[0][1] * aMatrix.myData[1][1] + myData[0][2] * aMatrix.myData[2][1],
							myData[0][0] * aMatrix.myData[0][2] + myData[0][1] * aMatrix.myData[1][2] + myData[0][2] * aMatrix.myData[2][2],
			
							myData[1][0] * aMatrix.myData[0][0] + myData[1][1] * aMatrix.myData[1][0] + myData[1][2] * aMatrix.myData[2][0],
							myData[1][0] * aMatrix.myData[0][1] + myData[1][1] * aMatrix.myData[1][1] + myData[1][2] * aMatrix.myData[2][1],
							myData[1][0] * aMatrix.myData[0][2] + myData[1][1] * aMatrix.myData[1][2] + myData[1][2] * aMatrix.myData[2][2],

							myData[2][0] * aMatrix.myData[0][0] + myData[2][1] * aMatrix.myData[1][0] + myData[2][2] * aMatrix.myData[2][0],
							myData[2][0] * aMatrix.myData[0][1] + myData[2][1] * aMatrix.myData[1][1] + myData[2][2] * aMatrix.myData[2][1],
							myData[2][0] * aMatrix.myData[0][2] + myData[2][1] * aMatrix.myData[1][2] + myData[2][2] * aMatrix.myData[2][2]);
	}

	template<class T>
	inline Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		return Vector3<T>(aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1), 
						  aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2),
						  aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3));
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		return (*this) = (*this) * aMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const T aScalar) const
	{
		return Matrix3x3<T>(myElements[0] * aScalar, myElements[1] * aScalar, myElements[2] * aScalar,
							myElements[3] * aScalar, myElements[4] * aScalar, myElements[5] * aScalar,
							myElements[6] * aScalar, myElements[7] * aScalar, myElements[8] * aScalar);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*=(const T aScalar)
	{
		return (*this) = (*this) * aScalar;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(void)
	{
		return Matrix3x3<T>(-myElements[0], -myElements[1], -myElements[2], 
							-myElements[3], -myElements[4], -myElements[5], 
							-myElements[6], -myElements[7], -myElements[8]);
	}

	template<class T>
	inline bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aMatrix) const 
	{
		return  (myElements[0] == aMatrix.myElements[0] &&
				myElements[1] == aMatrix.myElements[1] && 
				myElements[2] == aMatrix.myElements[2] && 
				myElements[3] == aMatrix.myElements[3] && 
				myElements[4] == aMatrix.myElements[4] && 
				myElements[5] == aMatrix.myElements[5] && 
				myElements[6] == aMatrix.myElements[6] && 
				myElements[7] == aMatrix.myElements[7] && 
				myElements[8] == aMatrix.myElements[8]);
	}

	template<class T>
	inline bool Matrix3x3<T>::operator!=(const Matrix3x3<T>& aMatrix) const
	{
		return !((*this) == aMatrix);
	}

	template<class T>
	inline T& Matrix3x3<T>::operator()(const unsigned int aRow, const unsigned int aColumn)
	{
		if (aRow > 0 && aRow <= DATASIZE && aColumn > 0 && aColumn <= DATASIZE)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix3x3 index out of range.","");
		return myData[0][0];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator()(const unsigned int aRow, const unsigned int aColumn) const
	{
		if (aRow > 0 && aRow <= DATASIZE && aColumn > 0 && aColumn <= DATASIZE)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix3x3 index out of range.","");
		return myData[0][0];
	}

	template<class T>
	inline T& Matrix3x3<T>::operator[](const unsigned int anIndex)
	{
		if (anIndex < 0 || anIndex >= ELEMENTSIZE)
		{
			SYSERROR("Matrix3x3 index out of range.","");
		}
		return myElements[anIndex];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator[](const unsigned int anIndex) const
	{
		if (anIndex < 0 || anIndex >= ELEMENTSIZE)
		{
			SYSERROR("Matrix3x3 index out of range.","");
		}
		return myElements[anIndex];
	}
	template<class T>
	inline const T(&Matrix3x3<T>::GetElements())[9]
	{
		return myElements;
	}
}

#undef DATASIZE
#undef ELEMENTSIZE