#pragma once

namespace CommonUtilities
{
	template<class T>
	class Singleton
	{
	public:
		static T& GetInstance()
		{
			static T instance;
			return instance;
		}
	protected:
		Singleton() = default;
		Singleton(const Singleton& aCopy) = delete;
		Singleton& operator=(const Singleton& aOther) = delete;
	};
}