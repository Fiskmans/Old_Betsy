#pragma once
#include "GrowingArray.hpp"

namespace CommonUtilities
{
	template <class T, int Size = 10, bool Safemode = true>
	class Stack
	{
	public:
		//Skapar en tom stack
		Stack();

		//Returnerar antal element i stacken
		int GetSize() const;

		//Returnerar det översta elementet i stacken. Kraschar med en assert om
		//stacken är tom.
		const T &GetTop() const;

		//Returnerar det översta elementet i stacken. Kraschar med en assert om
		//stacken är tom.
		T &GetTop();

		//Lägger in ett nytt element överst pEstacken
		void Push(const T &aValue);

		//Tar bort det översta elementet från stacken och returnerar det. Kraschar
		//med en assert om stacken är tom.
		T Pop();
	private:
		GrowingArray<T> myData;
	};


	template<class T, int Size, bool Safemode>
	inline Stack<T, Size, Safemode>::Stack()
	{
		myData.Init(Size, Safemode);
	}

	template<class T, int Size, bool Safemode>
	inline int Stack<T, Size, Safemode>::GetSize() const
	{
		return myData.Size();
	}

	template<class T, int Size, bool Safemode>
	inline const T& Stack<T, Size, Safemode>::GetTop() const
	{
		return myData.GetLast();
	}

	template<class T, int Size, bool Safemode>
	inline T& Stack<T, Size, Safemode>::GetTop()
	{
		return myData.GetLast();
	}

	template<class T, int Size, bool Safemode>
	inline void Stack<T, Size, Safemode>::Push(const T & aValue)
	{
		myData.Add(aValue);
	}

	template<class T, int Size, bool Safemode>
	inline T Stack<T, Size, Safemode>::Pop()
	{

		return myData.Pop();
	}

}