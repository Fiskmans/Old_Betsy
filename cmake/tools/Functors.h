#ifndef TOOLS_FUNCTORS
#define TOOLS_FUNCTORS

#include <functional>

#include "common/Macros.h"

namespace tools
{
	class ExecuteOnDestruct
	{
	public:
		inline ExecuteOnDestruct(std::function<void()> aFunction) : myFunction(aFunction) { }
		inline ExecuteOnDestruct(ExecuteOnDestruct&& aOther) noexcept : myFunction(aOther.myFunction) { aOther.Disable(); }
		inline ~ExecuteOnDestruct() { if (myFunction) { myFunction(); } }

		inline void Disable() { myFunction = nullptr; }
	private:
		std::function<void()> myFunction;
	};

	template<class T>
	class ExecuteOnConstruct
	{
	public:
		ExecuteOnConstruct(T&& aFunctor) { aFunctor(); }
	};

	class ExecuteOnConstructWithoutCTAD
	{
	public:
		ExecuteOnConstructWithoutCTAD(const std::function<void()>& aFunctor) { aFunctor(); }
	};
	

}

#define EXECUTE_ON_DESTRUCT(scope) tools::ExecuteOnDestruct CONCAT(executeOnDestruct, __LINE__)([&]() scope)
#define EXECUTE_ONCE(scope) static tools::ExecuteOnConstruct CONCAT(onceExecuter, __LINE__)([&]() scope)

#endif