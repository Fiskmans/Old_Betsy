#pragma once

namespace Math
{
	template<typename BASETYPE>
	class Imaginary
	{
	public:

		Imaginary();
		~Imaginary() = default;
		Imaginary(BASETYPE aValue);
		template<typename T>
		Imaginary(Imaginary<T> aOther);

		Imaginary<BASETYPE>& operator=(const BASETYPE& aValue);
		template<typename T>
		Imaginary<BASETYPE>& operator=(const Imaginary<T>& aOther);

		Imaginary<BASETYPE> operator*(const BASETYPE& aOther) const;
		template<typename T>
		BASETYPE operator*(const Imaginary<T>& aOther);

		Imaginary<BASETYPE>& operator*=(const BASETYPE& aOther);


		Imaginary<BASETYPE>& operator+(const BASETYPE& aOther) const;
		template<typename T>
		Imaginary<BASETYPE>& operator+(const Imaginary<T>& aOther) const;

		Imaginary<BASETYPE>& operator+=(const BASETYPE& aOther);
		template<typename T>
		Imaginary<BASETYPE>& operator+=(const Imaginary<T>& aOther);



		BASETYPE GetValue() const;
	private:

		BASETYPE myValue;
	};

	template<typename BASETYPE>
	inline Imaginary<BASETYPE>::Imaginary() : myValue()
	{
	}

	template<typename BASETYPE>
	inline Imaginary<BASETYPE>::Imaginary(BASETYPE aValue)
	{
		myValue = aValue;
	}

	template<typename BASETYPE>
	inline Imaginary<BASETYPE>& Imaginary<BASETYPE>::operator=(const BASETYPE& aValue)
	{
		myValue = aValue;
		return *this;
	}

	template<typename BASETYPE>
	inline Imaginary<BASETYPE> Imaginary<BASETYPE>::operator*(const BASETYPE& aOther) const
	{
		Imaginary<BASETYPE> res(*this);
		res *= aOther;
		return res;
	}

	template<typename BASETYPE>
	inline Imaginary<BASETYPE>& Imaginary<BASETYPE>::operator*=(const BASETYPE& aOther)
	{
		myValue *= aOther;
		return *this;
	}

	template<typename BASETYPE>
	inline Imaginary<BASETYPE>& Imaginary<BASETYPE>::operator+(const BASETYPE& aOther) const
	{
		Imaginary<BASETYPE> res(*this);
		res += aOther;
		return res;
	}

	template<typename BASETYPE>
	inline Imaginary<BASETYPE>& Imaginary<BASETYPE>::operator+=(const BASETYPE& aOther)
	{
		myValue += aOther;
		return *this;
	}

	template<typename BASETYPE>
	inline BASETYPE Imaginary<BASETYPE>::GetValue() const
	{
		return myValue;
	}

	template<typename BASETYPE>
	template<typename T>
	inline Imaginary<BASETYPE>::Imaginary(Imaginary<T> aOther)
	{
		myValue = aOther.GetValue();
	}

	template<typename BASETYPE>
	template<typename T>
	inline Imaginary<BASETYPE>& Imaginary<BASETYPE>::operator=(const Imaginary<T>& aOther)
	{
		myValue = aOther.GetValue();
		return *this;
	}

	template<typename BASETYPE>
	template<typename T>
	inline BASETYPE Imaginary<BASETYPE>::operator*(const Imaginary<T>& aOther)
	{
		return -(myValue * aOther.GetValue());
	}

	template<typename BASETYPE>
	template<typename T>
	inline Imaginary<BASETYPE>& Imaginary<BASETYPE>::operator+(const Imaginary<T>& aOther) const
	{
		Imaginary<BASETYPE> res(*this);
		res += aOther;
		return res;
	}

	template<typename BASETYPE>
	template<typename T>
	inline Imaginary<BASETYPE>& Imaginary<BASETYPE>::operator+=(const Imaginary<T>& aOther)
	{
		myValue += aOther.GetValue();
		return *this;
	}

	template<typename TYPE>
	Imaginary<TYPE> operator*(const TYPE& aValue, const Imaginary<TYPE>& aImaginary)
	{
		return aImaginary * aValue;
	}

	template<typename TYPE>
	Imaginary<TYPE> operator+(const TYPE& aValue, const Imaginary<TYPE>& aImaginary)
	{
		return aImaginary + aValue;
	}
}

typedef Math::Imaginary<float> ImagF;
typedef Math::Imaginary<double> ImagD;
typedef Math::Imaginary<int> ImagI;