#pragma once

#include <initializer_list>
#include <cassert>
#pragma warning(push)
#pragma warning(disable: 4244)
#include <type_traits>
#pragma warning(pop)

namespace CommonUtilities
{
	template <typename Type, int size, typename CountType = unsigned short, bool UseSafeModeFlag = true>
	class VectorOnStack
	{
	public:
		VectorOnStack();
		VectorOnStack(const Type& aAddOne);
		VectorOnStack(const VectorOnStack& aVectorOnStack);
		VectorOnStack(const std::initializer_list<Type>& aInitList);
		~VectorOnStack();
		VectorOnStack & operator=(const VectorOnStack& aVectorOnStack);

		inline const Type& operator[](const CountType& aIndex) const;
		inline Type& operator[](const CountType& aIndex);
		inline void Add(const Type& aObject);
		inline void Insert(CountType aIndex, const Type& aObject);
		inline void DeleteCyclic(const Type& aObject);
		inline void DeleteCyclicAtIndex(CountType aItemNumber);
		inline void RemoveCyclic(const Type& aObject);
		inline void RemoveCyclicAtIndex(CountType aItemNumber);
		inline void Clear();
		inline void DeleteAll();
		inline CountType Size() const;
	private:
		Type myData[size];
		CountType myCurrentSize;
	};

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack() :
		myCurrentSize (0)
	{

	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack(const Type& aFillValue)
	{
		for (auto& iterator : myData)
		{
			iterator = aFillValue;
		}
		myCurrentSize = size;
	}

	template< typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack(const VectorOnStack& aVectorOnStack)
	{
		if (UseSafeModeFlag)
		{
			for (CountType index = 0; index < aVectorOnStack.myCurrentSize; ++index)
			{
				myData[index] = aVectorOnStack.myData[index];
			}
			myCurrentSize = aVectorOnStack.myCurrentSize;
		}
		else
		{
			memcpy(this->myData, aVectorOnStack.myData, sizeof(Type) * size);
			myCurrentSize = aVectorOnStack.myCurrentSize;
		}
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack(const std::initializer_list<Type>& aInitList)
	{
		assert(aInitList.size() <= size && "initializer_list bigger than array size");
		
		myCurrentSize = 0;

		if (UseSafeModeFlag)
		{
			
			for (std::initializer_list<Type>::const_iterator iterator = aInitList.begin(); iterator != aInitList.end(); ++iterator)
			{
				myData[myCurrentSize] = *iterator;
				++myCurrentSize;
			}
		}
		else
		{
			if (aInitList.size() > 0)
			{
				if (size <= aInitList.size())
				{
					memcpy(myData, aInitList.begin(), sizeof(Type) * size);
					myCurrentSize = size;
				}
				else if (size > aInitList.size())
				{
					memcpy(myData, aInitList.begin(), sizeof(Type) * aInitList.size());
					myCurrentSize = aInitList.size();
				}
			}
		}
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, size, CountType, UseSafeModeFlag>::~VectorOnStack()
	{
		myCurrentSize = 0;
		for (CountType index = 0; index < size; ++index)
		{
			myData[index] = Type();
		}
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, size, CountType, UseSafeModeFlag>& VectorOnStack<Type, size, CountType, UseSafeModeFlag>::operator=(const VectorOnStack& aVectorOnStack)
	{
		if (UseSafeModeFlag)
		{
			for (CountType index = 0; index < aVectorOnStack.myCurrentSize; ++index)
			{
				myData[index] = aVectorOnStack.myData[index];
			}
			myCurrentSize = aVectorOnStack.myCurrentSize;
		}
		else
		{
			memcpy(this->myData, aVectorOnStack.myData, sizeof(Type) * size);
			myCurrentSize = aVectorOnStack.myCurrentSize;
		}
		return *this;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline const Type & VectorOnStack<Type, size, CountType, UseSafeModeFlag>::operator[](const CountType& aIndex) const
	{
		assert(aIndex >= 0 && aIndex < myCurrentSize && "Index out of bounds");

		return myData[aIndex];
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline Type& VectorOnStack<Type, size, CountType, UseSafeModeFlag>::operator[](const CountType& aIndex)
	{
		assert(aIndex >= 0 && aIndex < myCurrentSize && "Index out of bounds");
		
		return myData[aIndex];
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Add(const Type & aObject)
	{
		assert(myCurrentSize < size && "Array is full");
		
		myData[myCurrentSize] = aObject;
		++myCurrentSize;

	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Insert(CountType aIndex, const Type& aObject)
	{
		assert(myCurrentSize < size && "Array is full");
		assert(aIndex >= 0 && aIndex < myCurrentSize && "Out of bounds");

		if (UseSafeModeFlag)
		{
			for (int index = myCurrentSize; index > aIndex; --index)
			{
				myData[index] = myData[index - 1];
			}

			myData[aIndex] = aObject;
			++myCurrentSize;
		}
		else
		{
			memmove(myData + aIndex + 1, myData + aIndex, sizeof(Type) * (size - (aIndex + 1)));
			myData[aIndex] = aObject;
			++myCurrentSize;
		}
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::DeleteCyclic(const Type& aObject)
	{
		for (CountType index = 0; index < myCurrentSize; ++index)
		{
			if (myData[index] == aObject)
			{
				delete[] myData[index];
				myData[index] = myData[myCurrentSize - 1];
				--myCurrentSize;
				return;
			}
		}

		assert(false && "Could not find the object");
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::RemoveCyclic(const Type & aObject)
	{
		for (CountType index = 0; index < myCurrentSize; ++index)
		{
			if (myData[index] == aObject)
			{
				myData[index] = myData[myCurrentSize - 1];
				--myCurrentSize;
				return;
			}
		}

		assert(false && "Could not find the object");
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::DeleteCyclicAtIndex(CountType aItemNumber)
	{
		assert(aItemNumber >= 0 && aItemNumber < myCurrentSize && "Out of bounds");

		delete[] myData[aItemNumber];
		myData[aItemNumber] = myData[myCurrentSize - 1];
		--myCurrentSize;

	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::RemoveCyclicAtIndex(CountType aItemNumber)
	{
		assert(aItemNumber >= 0 && aItemNumber < myCurrentSize && "Out of bounds");

		myData[aItemNumber] = myData[myCurrentSize - 1];
		--myCurrentSize;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Clear()
	{
		myCurrentSize = 0;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::DeleteAll()
	{
		for (int index = 0; index < myCurrentSize; ++index)
		{
			delete[] myData[index];
			myData[index] = nullptr;
		}

		myCurrentSize = 0;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline CountType VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Size() const
	{
		return myCurrentSize;
	}
}

