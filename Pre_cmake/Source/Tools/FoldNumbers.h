#pragma once

namespace Math
{
	inline unsigned int BinaryFold(const void* aData)
	{
		static_assert(sizeof(void*) == sizeof(unsigned int) * 2);

		unsigned int val = *reinterpret_cast<const unsigned int*>(&aData);
		val ^= *(reinterpret_cast<const unsigned int*>(&aData) + 1);

		return val;
	}
}