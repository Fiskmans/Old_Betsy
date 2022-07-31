#ifndef TOOLS_FLOATCOMPARE_H
#define TOOLS_FLOATCOMPARE_H

#include <concepts>

namespace tools
{
	template<typename T>
	requires std::is_floating_point_v<T>
	class FloatCompare
	{
	public:
		FloatCompare(T avalue, T aEpsilon)
			: myValue(avalue)
			, myEpsilon(aEpsilon)
		{
		}

		template<typename TT>
		requires std::is_convertible_v<TT, T>
		bool operator== (const TT& aOther) const
		{
			return std::abs(myValue - aOther) < myEpsilon;
		}

	private:
		T myValue;
		T myEpsilon;
	};

	template<typename T, typename TT>
	requires std::is_floating_point_v<T> 
		&& std::is_floating_point_v<TT>
	bool operator==(const T& aLHS, const FloatCompare<TT>& aRHS)
	{
		return aRHS == aLHS;
	}
}

#endif