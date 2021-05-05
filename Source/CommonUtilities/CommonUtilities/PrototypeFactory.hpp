#pragma once
#include <memory>

namespace CommonUtilities
{
	template<typename T>
	class PrototypeFactory
	{
	public:
		PrototypeFactory() = default;

		template<typename... Args>
		inline void Load(T&& (*aInitFunction)(Args...), Args...);

		inline void Load(T&& aObject);

		const T& Object() const;
		T ManufactureCopy() const;
		T* ManufacturePointer() const;
		std::unique_ptr<T> ManufactureUniquePointer() const;

	private:
		T myObject;
	};

	template<typename T>
	inline const T& PrototypeFactory<T>::Object() const
	{
		return myObject;
	}

	template<typename T>
	inline T PrototypeFactory<T>::ManufactureCopy() const
	{
		return myObject;
	}

	template<typename T>
	inline T* PrototypeFactory<T>::ManufacturePointer() const
	{
		return new T(myObject);
	}

	template<typename T>
	inline std::unique_ptr<T> PrototypeFactory<T>::ManufactureUniquePointer() const
	{
		return std::make_unique<T>(myObject);
	}

	template<typename T>
	inline void PrototypeFactory<T>::Load(T&& aObject)
	{
		myObject = std::move(aObject);
	}

	template<typename T>
	template<typename... Args>
	inline void PrototypeFactory<T>::Load(T&& (*aInitFunction)(Args...), Args... args)
	{
		myObject = aInitFunction(args...);
	}
}