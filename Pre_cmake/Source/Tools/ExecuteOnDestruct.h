#pragma once

namespace Tools
{
	class ExecuteOnDestruct
	{
	public:
		ExecuteOnDestruct(std::function<void()> aFunction) : myFunction(aFunction) { }
		ExecuteOnDestruct(ExecuteOnDestruct&& aOther) noexcept : myFunction(aOther.myFunction) { aOther.Disable(); }
		~ExecuteOnDestruct() { if (myFunction) { myFunction(); } }

		void Disable() { myFunction = nullptr; }
	private:
		std::function<void()> myFunction;
	};
}