#pragma once

#include "GrowingArray.hpp"

namespace CommonUtilities
{
	template <class T, bool Safemode = true>
	class Queue
	{
	public:
		//Skapar en tom kö
		Queue();

		void ReInit(int aSize);

		//Returnerar antal element i kön
		int GetSize() const;

		//Returnerar elementet längst fram i kön. Kraschar med en assert om kön är 
		//tom
		const T& GetFront() const;

		//Returnerar elementet längst fram i kön. Kraschar med en assert om kön är 
		//tom
		T& GetFront();

		//Lägger in ett nytt element längst bak i kön
		void Enqueue(const T& aValue);

		//Tar bort elementet längst fram i kön och returnerar det. Kraschar med en 
		//assert om kön är tom.
		T Dequeue();
	private:
		GrowingArray<T> myData;
		int myBackIndex;
		int myFrontIndex;
		int mySize;
	};

	template<class T, bool Safemode>
	inline Queue<T, Safemode>::Queue() :
		myBackIndex (0),
		myFrontIndex (0),
		mySize(0)
	{
		myData.Init(10, Safemode);
		myData.Resize(10);
	}

	template<class T, bool Safemode>
	inline void Queue<T, Safemode>::ReInit(int aSize)
	{
		assert(aSize > 0 && "aSize <= 0");
		myData.ReInit(aSize);
		myData.Resize(aSize);
	}

	template<class T, bool Safemode>
	inline int Queue<T, Safemode>::GetSize() const
	{
		return mySize;
	}

	template<class T, bool Safemode>
	inline const T& Queue<T, Safemode>::GetFront() const
	{
		assert(myBackIndex > -1 && myBackIndex < myData.Size() && "Front index outside of array");
		assert(myFrontIndex > -1 && myFrontIndex < myData.Size() && "Back index outside of array");
		return myData[myFrontIndex];
	}

	template<class T, bool Safemode>
	inline T & Queue<T, Safemode>::GetFront()
	{
		assert(myBackIndex > -1 && myBackIndex < myData.Size() && "Front index outside of array");
		assert(myFrontIndex > -1 && myFrontIndex < myData.Size() && "Back index outside of array");
		return myData[myFrontIndex];
	}

	template<class T, bool Safemode>
	inline void Queue<T, Safemode>::Enqueue(const T& aValue)
	{
		if (mySize > 0)
		{
			++myBackIndex;
			if (myBackIndex == myData.Size())
			{
				myBackIndex = 0;
			}

			if (myBackIndex == myFrontIndex)
			{
				int size = myData.Size();
				myData.Resize(size * 2);
				if (Safemode)
				{
					int newFrontIndex = (myData.Size() - 1) - (size - 1 - myFrontIndex);
					for (int i = 0; i < size - myFrontIndex; ++i)
					{
						myData[newFrontIndex + i] = myData[myFrontIndex + i];
					}
					myFrontIndex = newFrontIndex;
				}
				else
				{

				}
			}
		}
		
		++mySize;

		myData[myBackIndex] = aValue;

	}

	template<class T, bool Safemode>
	inline T Queue<T, Safemode>::Dequeue()
	{
		assert(mySize > 0 && "Size <= 0");

		T* dataToRemove = &myData[myFrontIndex];

		--mySize;
		if (mySize > 0)
		{
			++myFrontIndex;
			if (myFrontIndex == myData.Size())
			{
				myFrontIndex = 0;
			}
		}
		return *dataToRemove;
	}

}