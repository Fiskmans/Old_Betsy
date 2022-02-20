#pragma once
#include "../../Tools/Logger.h"

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
		Matrix3x3(const std::initializer_list<T>& anInitList);
		~Matrix3x3() = default;

		static Matrix3x3<T> CreateRotationAroundX(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundY(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundZ(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundPointX(const Vector3<T> &aPoint, const T anAngle); 
		static Matrix3x3<T> CreateRotationAroundPointY(const Vector3<T> &aPoint, const T anAngle); 

		static Matrix3x3<T> CreateRotationFromDirection(const Vector3<T>& aDirection);

		static Matrix3x3<T> Identity();
	
		void RotateAroundX(const T anAngle);
		void RotateAroundY(const T anAngle);
		void RotateAroundZ(const T anAngle);
		void RotateAroundPointX(const Vector3<T> &aPoint, const T anAngle);
		void RotateAroundPointY(const Vector3<T> &aPoint, const T anAngle);

		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);
	
		//Operators
		Matrix3x3<T> &operator=(const Matrix3x3<T>& aMatrix);

		Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>& operator+=(const Matrix3x3<T>& aMatrix);
	
		Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>& operator-=(const Matrix3x3<T>& aMatrix);
	
		Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>& operator*=(const Matrix3x3<T>& aMatrix);
	
		Matrix3x3<T> operator*(const T aScalar) const;
		Matrix3x3<T>& operator*=(const T aScalar);

		Matrix3x3<T> operator/(const T aScalar) const;
		Matrix3x3<T>& operator/=(const T aScalar);

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
			T myData[3][3];
			T myElements[9];
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
		for (size_t index = 0; index < 9; index++)
		{
			myElements[index] = aMatrix.myElements[index];
		}
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix) : myElements{}
	{
		for (unsigned int row = 0; row < 3; ++row)
		{
			for (unsigned int column = 0; column < 3; ++column)
			{
				myData[row][column] = aMatrix(row + 1, column + 1);
			}
		}
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const std::initializer_list<T>& anInitList) : myElements{}
	{
		assert(anInitList.size() == 9);

		for (size_t i = 0; i < 9; ++i)
		{
			myElements[i] = *(anInitList.begin() + i);
		}
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return {	1,  0, 0, 
					0,  c, s,
					0, -s, c };
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return {	c, 0, -s,
					0, 1,  0,
					s, 0,  c };
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return {	 c, s, 0,
					-s, c, 0,
					 0, 0, 1 };
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundPointX(const Vector3<T>& aPoint, const T anAngle)
	{
		return	Matrix3x3<T>({ 1, 0, aPoint.x, 0, 1, aPoint.y, 0, 0, 1 }) *
				CreateRotationAroundX(anAngle) * 
				Matrix3x3<T>({ 1, 0, -aPoint.x, 0, 1, -aPoint.y, 0, 0, -aPoint.z });
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundPointY(const Vector3<T>& aPoint, const T anAngle)
	{
		return	Matrix3x3<T>({ 1, 0, aPoint.x, 0, 1, aPoint.y, 0, 0, 1 }) *
				CreateRotationAroundY(anAngle) * 
				Matrix3x3<T>({1, 0, -aPoint.x, 0, 1, -aPoint.y, 0, 0, 1});
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationFromDirection(const Vector3<T>& aDirection)
	{
		Vector3<T> forward = aDirection.GetNormalized();
		Vector3<T> right = forward.Cross(-Vector3<T>(0, 1, 0)).GetNormalized();
		Vector3<T> up = -right.Cross(forward);

		return {	right.x,	right.y,	right.z,
					up.x,		up.y,		up.z, 
					forward.x,	forward.y,	forward.z};
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Identity()
	{
		return Matrix3x3<T>();
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> transpose;

		for (size_t row = 0; row < 3; ++row)
		{
			for (size_t column = 0; column < 3; ++column)
			{
				transpose.myData[column][row] = aMatrixToTranspose.myData[row][column];
			}
		}

		return transpose;
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundX(const T anAngle)
	{
		(*this) *= CreateRotationAroundX(anAngle);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundY(const T anAngle)
	{
		(*this) *= CreateRotationAroundY(anAngle) ;
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundZ(const T anAngle)
	{
		(*this) *= CreateRotationAroundZ(anAngle);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundPointX(const Vector3<T>& aPoint, const T anAngle)
	{ 
		(*this) *= CreateRotationAroundPointX(aPoint, anAngle) ;
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundPointY(const Vector3<T>& aPoint, const T anAngle)
	{
		(*this) *= CreateRotationAroundPointY(aPoint, anAngle);
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		for (size_t index = 0; index < 9; ++index)
		{
			myElements[index] = aMatrix.myElements[index];
		}

		return (*this);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> copy(*this);
		copy += aMatrix;
		return copy;
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		for (size_t i = 0; i < sizeof(myElements) / sizeof(T); i++)
		{
			myElements[i] += aMatrix.myElements[i];
		}
		return *this;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> copy(*this);
		copy -= aMatrix;
		return copy;
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		for (size_t i = 0; i < sizeof(myElements) / sizeof(T); i++)
		{
			myElements[i] -= aMatrix.myElements[i];
		}
		return *this;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> out(*this);
		out *= aMatrix;
		return out;
	}

	template<class T>
	inline Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		return Vector3<T>(aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1), 
						  aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2),
						  aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3));
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
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
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const T aScalar) const
	{
		Matrix3x3<T> out(*this);
		out *= aScalar;
		return out;
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const T aScalar)
	{
		for (size_t i = 0; i < sizeof(myElements) / sizeof(T); i++)
		{
			myElements[i] *= aScalar;
		}
		return (*this);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator/(const T aScalar) const
	{
		return operator *(1 / aScalar);
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator/=(const T aScalar)
	{
		return operator *=(1 / aScalar);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(void)
	{
		return {	-myElements[0], -myElements[1], -myElements[2], 
					-myElements[3], -myElements[4], -myElements[5], 
					-myElements[6], -myElements[7], -myElements[8] };
	}

	template<class T>
	inline bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aMatrix) const
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
	inline bool Matrix3x3<float>::operator==(const Matrix3x3<float>& aMatrix) const
	{
		const float eps = 1e-10f;
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
	inline bool Matrix3x3<double>::operator==(const Matrix3x3<double>& aMatrix) const
	{
		const double eps = 1e-10;
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
	inline bool Matrix3x3<T>::operator!=(const Matrix3x3<T>& aMatrix) const
	{
		return !((*this) == aMatrix);
	}

	template<class T>
	inline T& Matrix3x3<T>::operator()(const unsigned int aRow, const unsigned int aColumn)
	{
		if (aRow > 0 && aRow <= 3 && aColumn > 0 && aColumn <= 3)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix3x3 index out of range.");
		return myData[0][0];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator()(const unsigned int aRow, const unsigned int aColumn) const
	{
		if (aRow > 0 && aRow <= 3 && aColumn > 0 && aColumn <= 3)
		{
			return myData[aRow - 1][aColumn - 1];
		}

		SYSERROR("Matrix3x3 index out of range.");
		return myData[0][0];
	}

	template<class T>
	inline T& Matrix3x3<T>::operator[](const unsigned int anIndex)
	{
		if (anIndex < 0 || anIndex >= 9)
		{
			SYSERROR("Matrix3x3 index out of range.");
		}
		return myElements[anIndex];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator[](const unsigned int anIndex) const
	{
		if (anIndex < 0 || anIndex >= 9)
		{
			SYSERROR("Matrix3x3 index out of range.");
		}
		return myElements[anIndex];
	}
	template<class T>
	inline const T(&Matrix3x3<T>::GetElements())[9]
	{
		return myElements;
	}
}

typedef CommonUtilities::Matrix3x3<float> M33f;
typedef CommonUtilities::Matrix3x3<double> M33d;
typedef CommonUtilities::Matrix3x3<int> M33i;