#pragma once
#include "Singleton.hpp"

namespace CommonUtilities
{
	template<typename Type>
	class SimpleUnique : public Singleton<SimpleUnique<Type>>
	{
		friend class Singleton<SimpleUnique<Type>>;
	public:
		SimpleUnique() :
			myNumber(0)
		{
		}

		Type Generate()
		{
			return ++myNumber;
		};

		void Reset()
		{
			new (&myNumber) Type();
		}
	private:
		Type myNumber;
	};
}