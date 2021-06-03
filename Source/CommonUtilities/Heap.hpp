#pragma once
#include "GrowingArray.hpp"
#include <functional>
namespace CommonUtilities
{
	template <class T, int size = 10, bool Safemode = true, class Compare = std::greater<T>>
	class Heap
	{
	public:
		Heap();
		int Size() const; 
		void Enqueue(const T &aElement); 
		const T& GetTop() const; 
		T Dequeue();
	private:
		
		GrowingArray<T> myData;
	};


	template<class T, int size, bool Safemode, class Compare>
	inline Heap<T, size, Safemode, Compare>::Heap()
	{
		myData.Init(size, Safemode);
	}

	template<class T, int size, bool Safemode, class Compare>
	inline int Heap<T, size, Safemode, Compare>::Size() const
	{
		return myData.Size();
	}

	template<class T, int size, bool Safemode, class Compare>
	inline const T& Heap<T, size, Safemode, Compare>::GetTop() const
	{
		assert(myData.Size() > 0 && "Size is not > 0");
		return myData[0];
	}

	template<class T, int size, bool Safemode, class Compare>
	inline void Heap<T, size, Safemode, Compare>::Enqueue(const T & aElement)
	{
		myData.Add(aElement);

		int i = myData.Size() - 1;

		while(i != 0 && Compare()(myData[i], myData[(i - 1) / 2]))
		{
			int parent = (i - 1) / 2;
			std::swap(myData[i], myData[parent]);
			i = parent;
		}

	}

	template<class T, int size, bool Safemode, class Compare>
	inline T Heap<T, size, Safemode, Compare>::Dequeue()
	{
		assert(myData.Size() > 0 && "Heap size not > 0");
		T returnValue = myData[0];
		myData[0] = myData[myData.Size() - 1];
		myData.Pop();

		int first = 0;
		int indexAtLowerTier = 1;
		while (indexAtLowerTier <= myData.Size() - 1)
		{
			
			if(indexAtLowerTier < myData.Size() - 1 && Compare()(myData[indexAtLowerTier + 1], myData[indexAtLowerTier]))
			{
				++indexAtLowerTier;
			}


			if(Compare()(myData[indexAtLowerTier], myData[first]))
			{
				std::swap(myData[indexAtLowerTier], myData[first]);
				first = indexAtLowerTier;
				indexAtLowerTier = 2 * first + 1;
			}
			else
			{
				indexAtLowerTier = myData.Size();
			}
		}

		return returnValue;
	}

}