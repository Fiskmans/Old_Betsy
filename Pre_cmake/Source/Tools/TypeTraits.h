#pragma once
#include <typeinfo>
#include <concepts>

namespace type_traits
{
	template<typename T>
	constexpr inline const char* Name()
	{
		return typeid(T).name() + 7;
	}

	template<typename T>
	requires std::is_class_v<T>
	constexpr inline const char* Name()
	{
		return typeid(T).name() + 6;
	}

	template<typename T>
	requires std::is_integral_v<T>
	constexpr inline const char* Name()
	{
		return typeid(T).name();
	}

}
