#pragma once
namespace CommonUtilities
{
	template<typename T>
	int Sign(T val)
	{
		return (T(0) < val) - (val < T(0));
	}
}