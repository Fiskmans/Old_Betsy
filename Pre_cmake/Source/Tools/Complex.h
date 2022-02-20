#pragma once
#include "Imaginary.h"

namespace Math
{

	template<typename BASETYPE>
	class Complex
	{
	public:
		Complex();
		~Complex() = default;
		Complex(BASETYPE aReal);
		template<typename T>
		Complex(Imaginary<T> aImaginary);
		template<typename T>
		Complex(BASETYPE aReal, Imaginary<T> aImaginary);
		template<typename T>
		Complex(Complex<T> aComplex);

		Complex<BASETYPE>& operator=(const BASETYPE& aValue);
		template<typename T>
		Complex<BASETYPE>& operator=(const Imaginary<T>& aImaginary);
		template<typename T>
		Complex<BASETYPE>& operator=(const Complex<T>& aComplex);

		Complex<BASETYPE> operator*(const BASETYPE& aReal) const;
		template<typename T>
		Complex<BASETYPE> operator*(const Imaginary<T>& aImaginary) const;
		template<typename T>
		Complex<BASETYPE> operator*(const Complex<T>& aOther) const;

		Complex<BASETYPE>& operator*=(const BASETYPE& aOther);
		template<typename T>
		Complex<BASETYPE>& operator*=(const Imaginary<T>& aOther);


		Complex<BASETYPE> operator+(const BASETYPE& aOther) const;
		template<typename T>
		Complex<BASETYPE> operator+(const Imaginary<T>& aOther) const;
		template<typename T>
		Complex<BASETYPE> operator+(const Complex<T>& aOther) const;


		Complex<BASETYPE>& operator+=(const BASETYPE& aReal);
		template<typename T>
		Complex<BASETYPE>& operator+=(const Imaginary<T>& aImaginary);
		template<typename T>
		Complex<BASETYPE>& operator+=(const Complex<T>& aOther);

		static Complex<BASETYPE> EPow(const Imaginary<BASETYPE>& aImaginary);
		BASETYPE Abs() const;


		BASETYPE GetRealPart() const;
		Imaginary<BASETYPE> GetImaginaryPart() const;
	private:

		BASETYPE myRealPart;
		Imaginary<BASETYPE> myImaginaryPart;
	};

	template<typename BASETYPE>
	inline Complex<BASETYPE>::Complex() : myRealPart(), myImaginaryPart()
	{
	}

	template<typename BASETYPE>
	inline Complex<BASETYPE>::Complex(BASETYPE aReal) : myRealPart(aReal) , myImaginaryPart()
	{
	}

	template<typename BASETYPE>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator=(const BASETYPE& aValue)
	{
		*this = Complex<BASETYPE>(aValue);
		return *this;
	}
	template<typename BASETYPE>
	inline Complex<BASETYPE> Complex<BASETYPE>::operator*(const BASETYPE& aReal) const
	{
		Complex<BASETYPE> out(*this);
		out *= aReal;
		return out;
	}
	template<typename BASETYPE>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator*=(const BASETYPE& aOther)
	{
		myRealPart *= aOther;
		myImaginaryPart *= aOther;
		return *this;
	}
	template<typename BASETYPE>
	inline Complex<BASETYPE> Complex<BASETYPE>::operator+(const BASETYPE& aOther) const
	{
		Complex<BASETYPE> out(*this);
		out.myRealPart += aOther;
		return out;
	}
	template<typename BASETYPE>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator+=(const BASETYPE& aReal)
	{
		myRealPart += aReal;
		return *this;
	}
	template<typename BASETYPE>
	inline BASETYPE Complex<BASETYPE>::GetRealPart() const
	{
		return myRealPart;
	}
	template<typename BASETYPE>
	inline Imaginary<BASETYPE> Complex<BASETYPE>::GetImaginaryPart() const
	{
		return myImaginaryPart;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>::Complex(Imaginary<T> aImaginary) : myRealPart(),myImaginaryPart(aImaginary)
	{
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>::Complex(BASETYPE aReal, Imaginary<T> aImaginary) : myRealPart(aReal),myImaginaryPart(aImaginary)
	{
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>::Complex(Complex<T> aComplex)
	{
		myRealPart = aComplex.GetRealPart();
		myImaginaryPart = aComplex.GetImaginaryPart();
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator=(const Imaginary<T>& aImaginary)
	{
		*this = Complex<BASETYPE>(aImaginary);
		return *this;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator=(const Complex<T>& aComplex)
	{
		*this = Complex<BASETYPE>(aComplex.GetRealPart(), aComplex.GetImaginaryPart());
		return *this;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE> Complex<BASETYPE>::operator*(const Imaginary<T>& aImaginary) const
	{
		Complex<BASETYPE> out(*this);
		out *= aImaginary;
		return out;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE> Complex<BASETYPE>::operator*(const Complex<T>& aOther) const
	{
		return aOther * myRealPart + aOther * myImaginaryPart;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator*=(const Imaginary<T>& aOther)
	{
		BASETYPE real = myRealPart;
		myRealPart = myImaginaryPart * aOther;
		myImaginaryPart = real * aOther;
		return *this;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE> Complex<BASETYPE>::operator+(const Imaginary<T>& aOther) const
	{
		Complex<BASETYPE> out(*this);
		out.myImaginaryPart += aOther;
		return out;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE> Complex<BASETYPE>::operator+(const Complex<T>& aOther) const
	{
		Complex<BASETYPE> out(*this);
		out += aOther;
		return out;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator+=(const Imaginary<T>& aImaginary)
	{
		myImaginaryPart += aImaginary;
		return *this;
	}
	template<typename BASETYPE>
	template<typename T>
	inline Complex<BASETYPE>& Complex<BASETYPE>::operator+=(const Complex<T>& aOther)
	{
		myRealPart += aOther.GetRealPart();
		myImaginaryPart += aOther.GetImaginaryPart();
		return *this;
	}

	template<typename BASETYPE>
	inline Complex<BASETYPE> Complex<BASETYPE>::EPow(const Imaginary<BASETYPE>& aImaginary)
	{
		return Complex<BASETYPE>(cos(aImaginary.GetValue()), Imaginary<BASETYPE>(sin(aImaginary.GetValue())));
	}

	template<typename BASETYPE>
	inline BASETYPE Complex<BASETYPE>::Abs() const
	{
		return sqrt(myRealPart * myRealPart + myImaginaryPart.GetValue() * myImaginaryPart.GetValue());
	}

	template<typename TYPE>
	Complex<TYPE> operator*(const TYPE& aValue, const Complex<TYPE>& acomplex)
	{
		return acomplex * aValue;
	}

	template<typename TYPE>
	Complex<TYPE> operator+(const TYPE& aValue, const Complex<TYPE>& acomplex)
	{
		return acomplex + aValue;
	}
}


typedef Math::Complex<float> CompF;
typedef Math::Complex<double> CompD;
typedef Math::Complex<int> CompI;