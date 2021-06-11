#pragma once

namespace Math
{
	template<class T>
	unsigned int BinaryFold(const T& aData)
	{
		static_assert(sizeof(aData) >= sizeof(unsigned int));

		unsigned int val = 0;

		for (size_t i = 0; i < (sizeof(aData) / sizeof(unsigned int)); i++)
		{
			val ^= *(reinterpret_cast<const unsigned int*>(&aData) + i);
		}

		return val;
	}
}