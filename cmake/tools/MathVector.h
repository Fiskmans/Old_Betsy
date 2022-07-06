#ifndef TOOLS_MATHVECTOR_H
#define TOOLS_MATHVECTOR_H

#include <math.h>
#include <utility>
#include <concepts>

#include "tools/Utility.h"

namespace tools
{
	template<typename Type, class DimensionSequence> 
	class MathVector_impl;

	template<typename Type, size_t DimensionIndex>
	using ExtractType = Type;

	template<class Type, std::size_t... DimensionIndexes>
	class MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>
	{
	public:
		static_assert(sizeof...(DimensionIndexes) > 1, "Thats just a scalar");

		MathVector_impl();
		MathVector_impl(const ExtractType<Type, DimensionIndexes>... aValues);
		MathVector_impl(const MathVector_impl& aVector) = default;

		template<std::size_t... OtherDimensionIndexes>
		MathVector_impl(const MathVector_impl<Type, std::index_sequence<OtherDimensionIndexes...>>& aOtherVector);

		template<std::convertible_to<Type>... ExtendedDimensions>
		MathVector_impl<Type, std::make_index_sequence<sizeof...(DimensionIndexes) + sizeof...(ExtendedDimensions)>> Extend(const ExtendedDimensions... aExtraValues);

		~MathVector_impl() = default;

		template<class OtherType>
		MathVector_impl<OtherType, std::index_sequence<DimensionIndexes...>> As();

		MathVector_impl& operator=(const MathVector_impl&aVector) = default;
		bool operator==(const MathVector_impl& aRHS) const;
		bool operator!=(const MathVector_impl& aRHS) const;

		Type& operator[](const size_t aDimension);
		Type operator[](const size_t aDimension) const;
		Type* Raw();
		Type const* Raw() const;

		void operator+=(const MathVector_impl& aRHS);
		MathVector_impl operator+(const MathVector_impl& aRHS) const;

		void operator-=(const MathVector_impl& aRHS);
		MathVector_impl operator-(const MathVector_impl& aRHS) const;

		void operator*=(const Type aValue);
		MathVector_impl operator*(const Type aValue) const;

		void operator*=(const MathVector_impl& aRHS);
		MathVector_impl operator*(const MathVector_impl& aRHS) const;

		void operator/=(const Type aValue);
		MathVector_impl operator/(const Type aValue) const;

		MathVector_impl operator-() const;

		Type Length() const;
		Type LengthSqr() const;
		Type Distance(const MathVector_impl& aVector) const;
		Type DistanceSqr(const MathVector_impl& aVector) const;
		void Normalize();
		MathVector_impl GetNormalized() const;

		template<typename Dimensions = std::index_sequence<DimensionIndexes...>>
		typename std::enable_if<Dimensions::size() == 3, MathVector_impl>::type Cross(const MathVector_impl& aOther);

		Type Dot(const MathVector_impl& aRHS) const;

		void ReflectPreNormalized(const MathVector_impl& aNormal);
		MathVector_impl ReflectedPreNormalized(const MathVector_impl& aNormal) const;

		void Reflect(const MathVector_impl& aNormal);
		MathVector_impl Reflected(const MathVector_impl& aNormal) const;


	private:
		Type myValues[sizeof...(DimensionIndexes)];
	};

	template<typename Type, size_t Dimensions>
	using MathVector = MathVector_impl<Type, std::make_index_sequence<Dimensions>>;

	using V2f = MathVector<float, 2>;
	using V2ui = MathVector<unsigned int, 2>;

	using V3f = MathVector<float, 3>;
	using V3ui = MathVector<unsigned int, 3>;

	using V4f = MathVector<float, 4>;
	using V4ui = MathVector<unsigned int, 4>;

	//**************************************************************************************

	template<class Type, std::size_t ...DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::MathVector_impl()
		: myValues{}
	{
	}
		
	template<class Type, std::size_t... DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::MathVector_impl(const ExtractType<Type, DimensionIndexes> ...aValues)
	{
		size_t index = 0;
		int _[]{ 0, (static_cast<void>(myValues[index++] = aValues), 0)... };
		static_cast<void>(_);
	}

	template<class Type, std::size_t... DimensionIndexes>
	template<std::size_t... OtherDimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::MathVector_impl(const MathVector_impl<Type, std::index_sequence<OtherDimensionIndexes...>>& aOther)
	{
		static_assert(sizeof...(DimensionIndexes) < sizeof...(OtherDimensionIndexes), "Cant construct vector with more dimensions than source");

		int _[]{ 0, (static_cast<void>(myValues[DimensionIndexes] = aOther[DimensionIndexes]), 0)... };
		static_cast<void>(_);
	}

	template<class Type, std::size_t ...DimensionIndexes>
	template<std::convertible_to<Type>... ExtendedDimensions>
	MathVector_impl<Type, std::make_index_sequence<sizeof...(DimensionIndexes) + sizeof...(ExtendedDimensions)>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Extend(const ExtendedDimensions... aExtraValues)
	{
		using ResultType = MathVector_impl<Type, std::make_index_sequence<sizeof...(DimensionIndexes) + sizeof...(ExtendedDimensions)>>;
		ResultType out;
		for (size_t i = 0; i < sizeof...(DimensionIndexes); i++)
			out[i] = myValues[i];

		size_t index = sizeof...(DimensionIndexes);

		int _[]{ 0, (static_cast<void>(out[index++] = static_cast<Type>(aExtraValues)), 0)... };
		static_cast<void>(_);

		return out;
	}

	template<class Type, std::size_t... DimensionIndexes>
	template<class OtherType>
	inline MathVector_impl<OtherType, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::As()
	{
		return MathVector_impl<OtherType, std::index_sequence<DimensionIndexes...>>(tools::Convert<Type, OtherType>(myValues[DimensionIndexes])...);
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline bool MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator==(const MathVector_impl& aRHS) const
	{
		return (Compare(this->myValues[DimensionIndexes], aRHS.myValues[DimensionIndexes]) &&...);
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline bool MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator!=(const MathVector_impl& aRHS) const
	{
		return !(*this == aRHS);
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline Type& MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator[](const size_t aIndex)
	{
		return myValues[aIndex];
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline Type MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator[](const size_t aIndex) const
	{
		return myValues[aIndex];
	}

	template<class Type, std::size_t ...DimensionIndexes>
	inline Type* MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Raw()
	{
		return myValues;
	}

	template<class Type, std::size_t ...DimensionIndexes>
	inline Type const* MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Raw() const
	{
		return myValues;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator+=(const MathVector_impl& aRHS)
	{
		for (size_t i = 0; i < sizeof...(DimensionIndexes); i++)
			myValues[i] += aRHS.myValues[i];
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator+(const MathVector_impl& aRHS) const
	{
		MathVector_impl copy(*this);
		copy += aRHS;
		return copy;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator-=(const MathVector_impl& aRHS)
	{
		for (size_t i = 0; i < sizeof...(DimensionIndexes); i++)
			myValues[i] -= aRHS.myValues[i];
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator-(const MathVector_impl& aRHS) const
	{
		MathVector_impl copy(*this);
		copy -= aRHS;
		return copy;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator*=(const Type aValue)
	{
		for (size_t i = 0; i < sizeof...(DimensionIndexes); i++)
			myValues[i] *= aValue;
	}

	template<class Type, std::size_t ...DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator*(const Type aValue) const
	{
		MathVector_impl copy(*this);
		copy *= aValue;
		return copy;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator*=(const MathVector_impl& aRHS)
	{
		for (size_t i = 0; i < sizeof...(DimensionIndexes); i++)
			myValues[i] *= aRHS[i];
	}

	template<class Type, std::size_t ...DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator*(const MathVector_impl& aRHS) const
	{
		MathVector_impl copy(*this);
		copy *= aRHS;
		return copy;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator/=(const Type aValue)
	{
		for (size_t i = 0; i < sizeof...(DimensionIndexes); i++)
			myValues[i] /= aValue;
	}

	template<class Type, std::size_t ...DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator/(const Type aValue) const
	{
		MathVector_impl copy(*this);
		copy /= aValue;
		return copy;
	}

	template<class Type, std::size_t ...DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::operator-() const
	{
		return MathVector_impl((-myValues[DimensionIndexes])...);
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline Type MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Length() const
	{
		return sqrt(LengthSqr());
	}
	
	template<class Type, std::size_t... DimensionIndexes>
	inline Type MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::LengthSqr() const
	{
		return (tools::Square(myValues[DimensionIndexes]) + ...);
	}
	
	template<class Type, std::size_t... DimensionIndexes>
	inline Type MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Distance(const MathVector_impl& aVector) const
	{
		return (*this - aVector).Length();
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline Type MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::DistanceSqr(const MathVector_impl& aVector) const
	{
		return (*this - aVector).LengthSqr();
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Normalize()
	{
		Type length = Length();

		for (Type& val : myValues)
			val /= length;
	}

	template<class Type, std::size_t ...DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::GetNormalized() const
	{
		MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> copy(*this);
		copy.Normalize();
		return copy;
	}

	template<class Type, std::size_t ...DimensionIndexes>
	template<typename Dimensions>
	inline typename std::enable_if<Dimensions::size() == 3, MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>>::type
	MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Cross(const MathVector_impl& aOther)
	{
		return MathVector_impl(
			(myValues[1] * aOther[2]) - (myValues[2] * aOther[1]),
			(myValues[2] * aOther[0]) - (myValues[0] * aOther[2]),
			(myValues[0] * aOther[1]) - (myValues[1] * aOther[0]));
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline Type MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Dot(const MathVector_impl& aRHS) const
	{
		Type sum{};

		for (size_t i = 0; i < sizeof...(DimensionIndexes); i++)
			sum += this->myValues[i] * aRHS.myValues[i];

		return sum;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::ReflectPreNormalized(const MathVector_impl& aNormal)
	{
		MathVector_impl reflectPoint = aNormal * Dot(aNormal);

		MathVector_impl delta = (*this) - reflectPoint;

		(*this) -= delta * 2;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::ReflectedPreNormalized(const MathVector_impl& aNormal) const
	{
		MathVector_impl copy(*this);
		copy.ReflectPreNormalized(aNormal);
		return copy;
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline void MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Reflect(const MathVector_impl& aNormal)
	{
		ReflectPreNormalized(aNormal.GetNormalized());
	}

	template<class Type, std::size_t... DimensionIndexes>
	inline MathVector_impl<Type, std::index_sequence<DimensionIndexes...>> MathVector_impl<Type, std::index_sequence<DimensionIndexes...>>::Reflected(const MathVector_impl& aNormal) const
	{
		return ReflectedPreNormalized(aNormal.GetNormalized());
	}
}

#endif