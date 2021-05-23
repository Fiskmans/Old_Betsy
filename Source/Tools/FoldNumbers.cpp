#include "pch.h"
#include "FoldNumbers.h"

unsigned int Math::FoldPointer(void* aPointer)
{
	static_assert(sizeof(aPointer) >= sizeof(unsigned int));

	unsigned int val;
	WIPE(val);

	for (size_t i = 0; i < (sizeof(aPointer) / sizeof(unsigned int)); i++)
	{
		unsigned int& inp = *(reinterpret_cast<unsigned int*>(&aPointer) + i);
		val ^= inp;
	}

	return val;
}
