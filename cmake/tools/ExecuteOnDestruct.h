#pragma once

#include <functional>

namespace Tools
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
}