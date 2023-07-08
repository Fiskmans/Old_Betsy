#ifndef TOOLS_MATRIX4X4_H
#define TOOLS_MATRIX4X4_H

#include "tools/MathVector.h"
#include "tools/Matrix3x3.h"

#include <cassert>

namespace tools
{

	template <class T>
	class Matrix4x4
	{
	public:
		Matrix4x4() = default;
		Matrix4x4(const Matrix4x4<T> &aMatrix) = default;
		Matrix4x4(const MathVector<T, 4> aRow1, const MathVector<T, 4> aRow2, const MathVector<T, 4> aRow3, const MathVector<T, 4> aRow4);
		Matrix4x4(
			const T a11, const T a12, const T a13, const T a14,
			const T a21, const T a22, const T a23, const T a24,
			const T a31, const T a32, const T a33, const T a34,
			const T a41, const T a42, const T a43, const T a44);

		~Matrix4x4() = default;

		static Matrix4x4<T> CreateRotationAroundX(const T anAngle);
		static Matrix4x4<T> CreateRotationAroundY(const T anAngle);
		static Matrix4x4<T> CreateRotationAroundZ(const T anAngle);
		static Matrix4x4<T> CreateRotation(const tools::MathVector<T, 3> aAngles);
		static Matrix4x4<T> CreateRotationAroundPointX(const T anAngle, const tools::MathVector<T, 4> &aPoint);
		static Matrix4x4<T> CreateRotationAroundPointY(const T anAngle, const tools::MathVector<T, 4> &aPoint);
		static Matrix4x4<T> CreateRotationAroundPointZ(const T anAngle, const tools::MathVector<T, 4> &aPoint);
		
		static Matrix4x4<T> CreateRotationFromDirection(const tools::MathVector<T, 3>& aDirection);

		static Matrix4x4<T> Identity();

		void RotateAroundX(const T anAngle);
		void RotateAroundY(const T anAngle);
		void RotateAroundZ(const T anAngle);
		void RotateAroundPointX(const T anAngle, const tools::MathVector<T, 4> &aPoint);
		void RotateAroundPointY(const T anAngle, const tools::MathVector<T, 4> &aPoint);
		void RotateAroundPointZ(const T anAngle, const tools::MathVector<T, 4> &aPoint);

		Matrix3x3<T> LowSubMatrix() const;

		Matrix4x4<T> Transposed() const;
		Matrix4x4<T>& Transpose();

		Matrix4x4<T> FastInverse() const;
		Matrix4x4<T> RealInverse() const;

		//Operators
		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix) = default;

		[[nodiscard]]
		Matrix4x4<T> operator+(const Matrix4x4<T> &aMatrix) const;
		void operator+=(const Matrix4x4<T> &aMatrix);

		[[nodiscard]]
		Matrix4x4<T> operator-(const Matrix4x4<T> &aMatrix) const;
		void operator-=(const Matrix4x4<T> &aMatrix);

		[[nodiscard]]
		Matrix4x4<T> operator*(const Matrix4x4<T> &aMatrix) const;
		void operator*=(const Matrix4x4<T> &aMatrix);

		[[nodiscard]]
		Matrix4x4<T> operator*(const T aScalar) const;
		void operator*=(const T aScalar);

		[[nodiscard]]
		Matrix4x4<T> operator/(const T aScalar) const;
		void operator/=(const T aScalar);

		[[nodiscard]]
		Matrix4x4<T> operator-(void);

		tools::MathVector<T, 4>& Row(size_t aIndex);
		const tools::MathVector<T, 4>& Row(size_t aIndex) const ;
		tools::MathVector<T, 4> Column(size_t aIndex) const;
		void AssignColumn(size_t aIndex, const tools::MathVector<T, 4>& aVector);

	private:

		MathVector<T, 4> myRows[4];
	};

	using M44f = Matrix4x4<float>;

	//*****************************************************************************

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const MathVector<T, 4> aRow1, const MathVector<T, 4> aRow2, const MathVector<T, 4> aRow3, const MathVector<T, 4> aRow4)
	{
		myRows[0] = aRow1;
		myRows[1] = aRow2;
		myRows[2] = aRow3;
		myRows[3] = aRow4;
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(
		const T a11, const T a12, const T a13, const T a14, 
		const T a21, const T a22, const T a23, const T a24, 
		const T a31, const T a32, const T a33, const T a34, 
		const T a41, const T a42, const T a43, const T a44)
	{
		myRows[0][0] = a11;
		myRows[0][1] = a12;
		myRows[0][2] = a13;
		myRows[0][3] = a14;

		myRows[1][0] = a21;
		myRows[1][1] = a22;
		myRows[1][2] = a23;
		myRows[1][3] = a24;

		myRows[2][0] = a31;
		myRows[2][1] = a32;
		myRows[2][2] = a33;
		myRows[2][3] = a34;

		myRows[3][0] = a41;
		myRows[3][1] = a42;
		myRows[3][2] = a43;
		myRows[3][3] = a44;
	}


	template<class T>
	inline tools::MathVector<T, 4>& Matrix4x4<T>::Row(size_t aIndex)
	{
		assert(aIndex < 4 && "accessing row out of range");
		return myRows[aIndex];
	}

	template<class T>
	inline const tools::MathVector<T, 4>& Matrix4x4<T>::Row(size_t aIndex) const
	{
		assert(aIndex < 4 && "accessing row out of range");
		return myRows[aIndex];
	}


	template<class T>
	inline tools::MathVector<T, 4> Matrix4x4<T>::Column(size_t aIndex) const
	{
		assert(aIndex < 4 && "accessing row out of range");
		return tools::MathVector<T, 4>(myRows[0][aIndex], myRows[1][aIndex], myRows[2][aIndex], myRows[3][aIndex]);
	}

	template<class T>
	inline void Matrix4x4<T>::AssignColumn(size_t aIndex, const tools::MathVector<T, 4>& aVector)
	{
		assert(aIndex < 4 && "accessing row out of range");
		myRows[0][aIndex] = aVector[0];
		myRows[1][aIndex] = aVector[1];
		myRows[2][aIndex] = aVector[2];
		myRows[3][aIndex] = aVector[3];
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix4x4(1, 0, 0, 0,
			0, c, s, 0,
			0, -s, c, 0,
			0, 0, 0, 1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix4x4(c, 0, -s, 0,
			0, 1, 0, 0,
			s, 0, c, 0,
			0, 0, 0, 1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T anAngle)
	{
		T c = static_cast<T>(cos(anAngle));
		T s = static_cast<T>(sin(anAngle));

		return Matrix4x4(c, -s, 0, 0,
			-s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotation(const tools::MathVector<T, 3> aAngles)
	{
		return
			CreateRotationAroundX(aAngles[0]) *
			CreateRotationAroundY(aAngles[1]) *
			CreateRotationAroundZ(aAngles[2]);

	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointX(const T anAngle, const tools::MathVector<T, 4>& aPoint)
	{
		return
			Matrix4x4<T>({ 1, 0, 0,  aPoint[0], 0, 1, 0,  aPoint[1], 0, 0, 1,  aPoint[2], 0, 0, 0, 1 }) *
			CreateRotationAroundX(anAngle) *
			Matrix4x4<T>({ 1, 0, 0, -aPoint[0], 0, 1, 0, -aPoint[1], 0, 0, 1, -aPoint[2], 0, 0, 0, 1 });
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointY(const T anAngle, const tools::MathVector<T, 4>& aPoint)
	{
		return
			Matrix4x4<T>({ 1, 0, 0,  aPoint[0], 0, 1, 0,  aPoint[1], 0, 0, 1,  aPoint[2], 0, 0, 0, 1 }) *
			CreateRotationAroundY(anAngle) *
			Matrix4x4<T>({ 1, 0, 0, -aPoint[0], 0, 1, 0, -aPoint[1], 0, 0, 1, -aPoint[2], 0, 0, 0, 1 });
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundPointZ(const T anAngle, const tools::MathVector<T, 4>& aPoint)
	{
		return
			Matrix4x4<T>({ 1, 0, 0,  aPoint[0], 0, 1, 0,  aPoint[1], 0, 0, 1,  aPoint[2], 0, 0, 0, 1 }) *
			CreateRotationAroundZ(anAngle) *
			Matrix4x4<T>({ 1, 0, 0, -aPoint[0], 0, 1, 0, -aPoint[1], 0, 0, 1, -aPoint[2], 0, 0, 0, 1 });
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationFromDirection(const tools::MathVector<T, 3>& aDirection)
	{
		tools::MathVector<T, 3> forward = aDirection.GetNormalized();
		tools::MathVector<T, 3> right = forward.Cross(-tools::MathVector<T, 3>(0, 1, 0)).GetNormalized();
		tools::MathVector<T, 3> up = -right.Cross(forward);

		return Matrix4x4(right[0], right[1], right[2], 0,
			up[0], up[1], up[2], 0,
			forward[0], forward[1], forward[2], 0,
			0, 0, 0, 1);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Identity()
	{
		return Matrix4x4<T>(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1);
	}


	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Transposed() const
	{
		Matrix4x4<T> transpose;

		for (size_t row = 0; row < 4; ++row)
		{
			for (size_t column = 0; column < 4; ++column)
			{
				transpose.myRows[column][row] = myRows[row][column];
			}
		}

		return transpose;
	}
	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::Transpose()
	{
		Matrix4x4<T> transpose(*this);
		transpose.Transpose();
		*this = transpose;

		return *this;
	}

	template<class T>
	 Matrix4x4<T> Matrix4x4<T>::FastInverse() const
	{
		tools::Matrix3x3<T> rot = LowSubMatrix();
		tools::Matrix3x3<T> trot = rot.Transposed();

		return Matrix4x4(	trot.Row(0).Extend(myRows[0][3]),
							trot.Row(1).Extend(myRows[1][3]),
							trot.Row(2).Extend(myRows[2][3]),	
							(MathVector<T, 3>(myRows[3]) * rot).Extend(myRows[3][3]));
	}

	template<class T>
	inline  Matrix4x4<T> Matrix4x4<T>::RealInverse() const
	{
		T A2323 = myRows[2][2] * myRows[3][3] - myRows[2][3] * myRows[3][2];
		T A1323 = myRows[2][1] * myRows[3][3] - myRows[2][3] * myRows[3][1];
		T A1223 = myRows[2][1] * myRows[3][2] - myRows[2][2] * myRows[3][1];
		T A0323 = myRows[2][0] * myRows[3][3] - myRows[2][3] * myRows[3][0];
		T A0223 = myRows[2][0] * myRows[3][2] - myRows[2][2] * myRows[3][0];
		T A0123 = myRows[2][0] * myRows[3][1] - myRows[2][1] * myRows[3][0];
		T A2313 = myRows[1][2] * myRows[3][3] - myRows[1][3] * myRows[3][2];
		T A1313 = myRows[1][1] * myRows[3][3] - myRows[1][3] * myRows[3][1];
		T A1213 = myRows[1][1] * myRows[3][2] - myRows[1][2] * myRows[3][1];
		T A2312 = myRows[1][2] * myRows[2][3] - myRows[1][3] * myRows[2][2];
		T A1312 = myRows[1][1] * myRows[2][3] - myRows[1][3] * myRows[2][1];
		T A1212 = myRows[1][1] * myRows[2][2] - myRows[1][2] * myRows[2][1];
		T A0313 = myRows[1][0] * myRows[3][3] - myRows[1][3] * myRows[3][0];
		T A0213 = myRows[1][0] * myRows[3][2] - myRows[1][2] * myRows[3][0];
		T A0312 = myRows[1][0] * myRows[2][3] - myRows[1][3] * myRows[2][0];
		T A0212 = myRows[1][0] * myRows[2][2] - myRows[1][2] * myRows[2][0];
		T A0113 = myRows[1][0] * myRows[3][1] - myRows[1][1] * myRows[3][0];
		T A0112 = myRows[1][0] * myRows[2][1] - myRows[1][1] * myRows[2][0];

		T det =	  myRows[0][0] * (myRows[1][1] * A2323 - myRows[1][2] * A1323 + myRows[1][3] * A1223)
				- myRows[0][1] * (myRows[1][0] * A2323 - myRows[1][2] * A0323 + myRows[1][3] * A0223)
				+ myRows[0][2] * (myRows[1][0] * A1323 - myRows[1][1] * A0323 + myRows[1][3] * A0123)
				- myRows[0][3] * (myRows[1][0] * A1223 - myRows[1][1] * A0223 + myRows[1][2] * A0123);
		det = 1.0f / det;


		return Matrix4x4(
			det *  (myRows[1][1] * A2323 - myRows[1][2] * A1323 + myRows[1][3] * A1223),
			det * -(myRows[0][1] * A2323 - myRows[0][2] * A1323 + myRows[0][3] * A1223),
			det *  (myRows[0][1] * A2313 - myRows[0][2] * A1313 + myRows[0][3] * A1213),
			det * -(myRows[0][1] * A2312 - myRows[0][2] * A1312 + myRows[0][3] * A1212),

			det * -(myRows[1][0] * A2323 - myRows[1][2] * A0323 + myRows[1][3] * A0223),
			det *  (myRows[0][0] * A2323 - myRows[0][2] * A0323 + myRows[0][3] * A0223),
			det * -(myRows[0][0] * A2313 - myRows[0][2] * A0313 + myRows[0][3] * A0213),
			det *  (myRows[0][0] * A2312 - myRows[0][2] * A0312 + myRows[0][3] * A0212),

			det *  (myRows[1][0] * A1323 - myRows[1][1] * A0323 + myRows[1][3] * A0123),
			det * -(myRows[0][0] * A1323 - myRows[0][1] * A0323 + myRows[0][3] * A0123),
			det *  (myRows[0][0] * A1313 - myRows[0][1] * A0313 + myRows[0][3] * A0113),
			det * -(myRows[0][0] * A1312 - myRows[0][1] * A0312 + myRows[0][3] * A0112),

			det * -(myRows[1][0] * A1223 - myRows[1][1] * A0223 + myRows[1][2] * A0123),
			det *  (myRows[0][0] * A1223 - myRows[0][1] * A0223 + myRows[0][2] * A0123),
			det * -(myRows[0][0] * A1213 - myRows[0][1] * A0213 + myRows[0][2] * A0113),
			det *  (myRows[0][0] * A1212 - myRows[0][1] * A0212 + myRows[0][2] * A0112));
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
	inline void Matrix4x4<T>::RotateAroundPointX(const T anAngle, const tools::MathVector<T, 4>& aPoint)
	{
		(*this) *= CreateRotationAroundPointX(anAngle, aPoint);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointY(const T anAngle, const tools::MathVector<T, 4>& aPoint)
	{
		(*this) *= CreateRotationAroundPointY(anAngle, aPoint);
	}

	template<class T>
	inline void Matrix4x4<T>::RotateAroundPointZ(const T anAngle, const tools::MathVector<T, 4>& aPoint)
	{
		(*this) *= CreateRotationAroundPointZ(anAngle, aPoint);
	}

	template<class T>
	inline Matrix3x3<T> Matrix4x4<T>::LowSubMatrix() const
	{
		return Matrix3x3<T>(myRows[0], myRows[1], myRows[2]);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> copy(*this);
		copy += aMatrix;
		return copy;
	}

	template<class T>
	inline void Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		for (size_t i = 0; i < sizeof(myRows) / sizeof(T); i++)
		{
			myRows[i] += aMatrix.myRows[i];
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
	inline void Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		for (size_t i = 0; i < sizeof(myRows) / sizeof(T); i++)
		{
			myRows[i] -= aMatrix.myRows[i];
		}
		return *this;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> out;
		
		for (size_t x = 0; x < 4; x++)
		{
			MathVector<T, 4> column = aMatrix.Column(x);
			for (size_t y = 0; y < 4; y++)
			{
				out.Row(y)[x] = column.Dot(Row(y));
			}
		}

		return out;
	}

	template<class T>
	inline tools::MathVector<T, 4> operator*(const tools::MathVector<T, 4>& aVector, const Matrix4x4<T>& aMatrix)
	{
		return tools::MathVector<T, 4>(aVector.Dot(aMatrix.Row(0)), aVector.Dot(aMatrix.Row(1)), aVector.Dot(aMatrix.Row(2)), aVector.Dot(aMatrix.Row(3)));
	}

	template<class T>
	inline void Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		(*this) = (*this) * aMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const T aScalar) const
	{
		Matrix4x4<T> out(*this);
		out *= aScalar;
		return out;
	}

	template<class T>
	inline void Matrix4x4<T>::operator*=(const T aScalar)
	{
		for (MathVector<T,4>& row : myRows)
			row *= aScalar;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator/(const T aScalar) const
	{
		return operator *(1 / aScalar);
	}

	template<class T>
	inline void Matrix4x4<T>::operator/=(const T aScalar)
	{
		operator *=(1 / aScalar);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(void)
	{
		return Matrix4x4<T>(-myRows[0][0], -myRows[0][1], -myRows[0][2], -myRows[0][3],
							-myRows[1][0], -myRows[1][1], -myRows[1][2], -myRows[1][3],
							-myRows[2][0], -myRows[2][1], -myRows[2][2], -myRows[2][3],
							-myRows[3][0], -myRows[3][1], -myRows[3][2], -myRows[3][3]);
	}
}

#endif