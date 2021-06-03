#pragma once
#include "ObjectPool.hpp"
#include "Singleton.hpp"
#include "GrowingArray.hpp"
#include <cassert>

namespace CommonUtilities
{

	template<typename T, unsigned int S>
	class ObjectPoolComplex : public Singleton<ObjectPoolComplex<T,S>>
	{
		friend class Singleton<ObjectPoolComplex<T, S>>;

	public:

		ObjectPool<T, S>& GetPool(unsigned int aIndex);
		void Init(unsigned int aSize, bool aSafeMode = true);
	private:
		GrowingArray<ObjectPool<T, S>> myPool;
	};

	template<typename T, unsigned int S>
	inline ObjectPool<T, S>& ObjectPoolComplex<T, S>::GetPool(unsigned int aIndex)
	{
		assert(aIndex >= 0 && aIndex < myPool.Size());

		return myPool[aIndex];
	}

	template<typename T, unsigned int S>
	inline void ObjectPoolComplex<T, S>::Init(unsigned int aSize, bool aSafeMode)
	{
		assert(myPool.ReservedSize() == 0);

		myPool.Init(aSize, aSafeMode);
		myPool.Resize(aSize);
	}
}

