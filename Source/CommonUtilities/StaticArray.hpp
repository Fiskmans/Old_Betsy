#pragma once

#include <initializer_list>
#include <cassert>
#pragma warning(push)
#pragma warning(disable: 4244)
#include <type_traits>
#pragma warning(pop)

namespace CommonUtilities
{
	template <typename Type, int size>
	class StaticArray
	{
	public:
		StaticArray() = default;
		StaticArray(const StaticArray& aStaticArray);
		StaticArray(const std::initializer_list<Type>& aInitList);
		~StaticArray();
		StaticArray& operator=(const StaticArray& aStaticArray);
		inline const Type& operator[](const int& aIndex) const;
		inline Type& operator[](const int& aIndex);

		inline void Insert(int aIndex, const Type& aObject);
		inline void DeleteAll();
	private:
		Type myData[size];
	};

	template<typename Type, int size>
	StaticArray<Type, size>::StaticArray(const StaticArray& aStaticArray)
	{
		for (size_t index = 0; index < size; index++)
		{
			myData[index] = aStaticArray.myData[index];
		}
	}

	template<typename Type, int size>
	StaticArray<Type, size>::~StaticArray()
	{

	}

	template<typename Type, int size>
	StaticArray<Type, size>::StaticArray(const std::initializer_list<Type>& aInitList)
	{
		assert(aInitList.size() <= size && "initializer_list bigger than array size");

		if (aInitList.size() != 1)
		{
			int count = 0;
			for (std::initializer_list<Type>::const_iterator iterator = aInitList.begin(); iterator != aInitList.end(); ++iterator)
			{
				myData[count] = *iterator;
				++count;
			}
			if (count < size - 1 && aInitList.size() > 0)
			{
				for (size_t index = count; index < size; ++index)
				{
					myData[index] = *(aInitList.end() - 1);
				}
			}
		}
		else
		{
			for (auto& iterator : myData)
			{
				iterator = *aInitList.begin();
			}
		}
	}

	template<typename Type, int size>
	StaticArray<Type, size>& StaticArray<Type, size>::operator=(const StaticArray& aStaticArray)
	{
		for (size_t index = 0; index < size; index++)
		{
			myData[index] = aStaticArray.myData[index];
		}
		return *this;
	}

	template<typename Type, int size>
	inline const Type & StaticArray<Type, size>::operator[](const int& aIndex) const
	{
		assert(aIndex >= 0 && aIndex < size);

		return myData[aIndex];
	}

	template<typename Type, int size>
	inline Type& StaticArray<Type, size>::operator[](const int& aIndex)
	{
		assert(aIndex >= 0 && aIndex < size);

		return myData[aIndex];
	}

	template<typename Type, int size>
	inline void StaticArray<Type, size>::Insert(int aIndex, const Type& aObject)
	{
		assert(aIndex >= 0 && aIndex < size);
		
		for (int index = size - 1; index > aIndex; --index)
		{
			myData[index] = myData[index - 1];
		}
		
		myData[aIndex] = aObject;

	}

	template<typename Type, int size>
	inline void StaticArray<Type, size>::DeleteAll()
	{
		for (int index = 0; index < size; ++index)
		{
			delete[] myData[index];
			myData[index] = nullptr;
		}
	}
}