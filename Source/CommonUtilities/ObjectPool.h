#pragma once


namespace CommonUtilities
{
	template<class T>
	class ObjectPool
	{
	public:

		typedef std::vector<T*>::iterator iterator;

		template<class... Args>
		T* Get(Args... args);

		void Return(T* aObject);

		iterator begin();
		iterator end();

	private:

		std::vector<T*> mySlots;
	};

	template<class T>
	template<class... Args>
	inline T* ObjectPool<T>::Get(Args... args)
	{
		T* out = new T(args...);
		mySlots.push_back(out);
		return out;
	}

	template<class T>
	inline void ObjectPool<T>::Return(T* aObject)
	{
		mySlots.erase(std::find(mySlots.begin(), mySlots.end(), aObject));
		delete aObject;
	}


	template<class T>
	inline ObjectPool<T>::iterator ObjectPool<T>::begin()
	{
		return mySlots.begin();
	}

	template<class T>
	inline ObjectPool<T>::iterator ObjectPool<T>::end()
	{
		return mySlots.end();
	}
}