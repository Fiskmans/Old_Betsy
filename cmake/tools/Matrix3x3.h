#ifndef TOOLS_MATRIX3X3_H
#define TOOLS_MATRIX3X3_H

#include "tools/MathVector.h"

namespace tools
{

	template <class T>
	class Matrix3x3
	{
	public:
		Matrix3x3() = default;
		Matrix3x3(const Matrix3x3<T> &aMatrix);
		Matrix3x3(const MathVector<T, 3> aRow1, const MathVector<T, 3> aRow2, const MathVector<T, 3> aRow3);

		Matrix3x3(
			const T a11, const T a12, const T a13,
			const T a21, const T a22, const T a23,
			const T a31, const T a32, const T a33);

		~Matrix3x3() = default;

		static Matrix3x3<T> CreateRotationAroundX(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundY(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundZ(const T anAngle);
		static Matrix3x3<T> CreateRotationAroundPointX(const tools::MathVector<T, 3> &aPoint, const T anAngle); 
		static Matrix3x3<T> CreateRotationAroundPointY(const tools::MathVector<T, 3> &aPoint, const T anAngle); 

		static Matrix3x3<T> CreateRotationFromDirection(const tools::MathVector<T, 3>& aDirection);

		static Matrix3x3<T> Identity();
	
		void RotateAroundX(const T anAngle);
		void RotateAroundY(const T anAngle);
		void RotateAroundZ(const T anAngle);
		void RotateAroundPointX(const tools::MathVector<T, 3> &aPoint, const T anAngle);
		void RotateAroundPointY(const tools::MathVector<T, 3> &aPoint, const T anAngle);

		Matrix3x3<T> Transposed() const;
		Matrix3x3<T>& Transpose();
	
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

		MathVector<T, 3>& Row(size_t aIndex);
		const MathVector<T, 3>& Row(size_t aIndex) const;

		MathVector<T, 3> Column(size_t aIndex);
		const MathVector<T, 3> Column(size_t aIndex) const;
	private:

		MathVector<T, 3> myRows[3];
	};

	using M33f = Matrix3x3<float>;

	//*****************************************************************************

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T> &aMatrix) : myRows{}
	{
		for (size_t y = 0; y < 3; y++)
		{
			myRows[y] = aMatrix.myRows[y];
		}
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const MathVector<T, 3> aRow1, const MathVector<T, 3> aRow2, const MathVector<T, 3> aRow3)
	{
		myRows[0] = aRow1;
		myRows[1] = aRow2;
		myRows[2] = aRow3;
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(
		const T a11, const T a12, const T a13,
		const T a21, const T a22, const T a23, 
		const T a31, const T a32, const T a33)
	{
		myRows[0][0] = a11;
		myRows[0][1] = a12;
		myRows[0][2] = a13;

		myRows[1][0] = a21;
		myRows[1][1] = a22;
		myRows[1][2] = a23;
		
		myRows[2][0] = a31;
		myRows[2][1] = a32;
		myRows[2][2] = a33;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix3x3(	1,  0, 0,
							0,  c, s,
							0, -s, c );
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix3x3(	c, 0, -s,
							0, 1, 0,
							s, 0, c);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix3x3(	c, s, 0,
							-s, c, 0,
							0, 0, 1);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundPointX(const tools::MathVector<T, 3>& aPoint, const T anAngle)
	{
		return	Matrix3x3<T>({ 1, 0, aPoint.x, 0, 1, aPoint.y, 0, 0, 1 }) *
			CreateRotationAroundX(anAngle) *
			Matrix3x3<T>({ 1, 0, -aPoint.x, 0, 1, -aPoint.y, 0, 0, -aPoint.z });
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundPointY(const tools::MathVector<T, 3>& aPoint, const T anAngle)
	{
		return	Matrix3x3<T>({ 1, 0, aPoint.x, 0, 1, aPoint.y, 0, 0, 1 }) *
			CreateRotationAroundY(anAngle) *
			Matrix3x3<T>({ 1, 0, -aPoint.x, 0, 1, -aPoint.y, 0, 0, 1 });
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationFromDirection(const tools::MathVector<T, 3>& aDirection)
	{
		tools::MathVector<T, 3> forward = aDirection.GetNormalized();
		tools::MathVector<T, 3> right = forward.Cross(-tools::MathVector<T, 3>(0, 1, 0)).GetNormalized();
		tools::MathVector<T, 3> up = -right.Cross(forward);

		return Matrix3x3(	right.x, right.y, right.z,
							up.x, up.y, up.z,
							forward.x, forward.y, forward.z);
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Identity()
	{
		return Matrix3x3<T>(	1, 0, 0,
								0, 1, 0,
								0, 0, 1);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundX(const T anAngle)
	{
		(*this) *= CreateRotationAroundX(anAngle);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundY(const T anAngle)
	{
		(*this) *= CreateRotationAroundY(anAngle);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundZ(const T anAngle)
	{
		(*this) *= CreateRotationAroundZ(anAngle);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundPointX(const tools::MathVector<T, 3>& aPoint, const T anAngle)
	{
		(*this) *= CreateRotationAroundPointX(aPoint, anAngle);
	}

	template<class T>
	inline void Matrix3x3<T>::RotateAroundPointY(const tools::MathVector<T, 3>& aPoint, const T anAngle)
	{
		(*this) *= CreateRotationAroundPointY(aPoint, anAngle);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transposed() const
	{
		Matrix3x3<T> transpose;

		for (size_t row = 0; row < 3; ++row)
		{
			for (size_t column = 0; column < 3; ++column)
			{
				transpose.myRows[column][row] = myRows[row][column];
			}
		}

		return transpose;
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::Transpose()
	{
		Matrix3x3<T> transposed(*this);
		transposed.Transpose();
		*this = transposed;

		return *this;
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		for (size_t index = 0; index < 9; ++index)
		{
			myRows[index] = aMatrix.myRows[index];
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
		for (size_t i = 0; i < sizeof(myRows) / sizeof(T); i++)
		{
			myRows[i] += aMatrix.myRows[i];
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
		for (size_t i = 0; i < sizeof(myRows) / sizeof(T); i++)
		{
			myRows[i] -= aMatrix.myRows[i];
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
	inline tools::MathVector<T, 3> operator*(const tools::MathVector<T, 3>& aVector, const Matrix3x3<T>& aMatrix)
	{
		return tools::MathVector<T, 3>(aVector.Dot(aMatrix.Column(0)), aVector.Dot(aMatrix.Column(1)), aVector.Dot(aMatrix.Column(2)));
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		T data[16];
		memcpy(data, myRows, 16 * sizeof(float));

		for (size_t i = 0; i < 16; i++)
		{
			myRows[i] = 0;
			size_t x = i % 4;
			size_t y = i / 4;

			for (size_t ii = 0; ii < 4; ii++)
			{
				myRows[i] += data[y * 4 + ii] * aMatrix.myRows[ii * 4 + x];
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
		for (size_t i = 0; i < sizeof(myRows) / sizeof(T); i++)
		{
			myRows[i] *= aScalar;
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
	inline MathVector<T, 3>& Matrix3x3<T>::Row(size_t aIndex)
	{
		return myRows[aIndex];
	}

	template<class T>
	inline const MathVector<T, 3>& Matrix3x3<T>::Row(size_t aIndex) const
	{
		return myRows[aIndex];
	}
	template<class T>
	inline MathVector<T, 3> Matrix3x3<T>::Column(size_t aIndex)
	{
		return MathVector<T, 3>(myRows[0][aIndex], myRows[1][aIndex], myRows[2][aIndex]);
	}
	template<class T>
	inline const MathVector<T, 3> Matrix3x3<T>::Column(size_t aIndex) const
	{
		return MathVector<T, 3>(myRows[0][aIndex], myRows[1][aIndex], myRows[2][aIndex]);
	}
}

#endif