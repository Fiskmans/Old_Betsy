#pragma once

#include <cassert>
#include <memory>

namespace CommonUtilities
{
	template<typename Type, typename SizeType = int>
	class GrowingArray
	{
	public:

		class Iterator
		{
		public:
			Iterator(Type* aPointer) : myPointer(aPointer) {}
			Iterator operator++() { ++myPointer; return *this; }
			bool operator!=(const Iterator& other) const { return myPointer != other.myPointer; }
			Type& operator*() { return *myPointer; }
		private:
			Type* myPointer;
		};

		class ReverseIterator
		{
		public:
			ReverseIterator(Type* aPointer) : myPointer(aPointer) {}
			ReverseIterator operator++() { --myPointer; return *this; }
			bool operator!=(const ReverseIterator& other) const { return myPointer != other.myPointer; }
			Type& operator*() { return *myPointer; }
		private:
			Type* myPointer;
		};

		class ReverseAdapter
		{
		public:
			ReverseAdapter(GrowingArray* aArray) : myArray(aArray) {}
			inline ReverseIterator begin() { return myArray->rbegin(); }
			inline ReverseIterator end() { return myArray->rend(); }

		private:
			GrowingArray* myArray;
		};

		GrowingArray();
		GrowingArray(SizeType aNrOfRecommendedItems, bool aUseSafeMode = true);
		GrowingArray(const GrowingArray& aGrowingArray);
		GrowingArray(GrowingArray&& aGrowingArray);
		inline GrowingArray(const std::initializer_list<Type>& aInitList, bool aUseSafeMode = true);
		~GrowingArray();
		GrowingArray& operator=(const GrowingArray& aGrowingArray);
		GrowingArray& operator=(GrowingArray&& aGrowingArray);
		void Init(SizeType aInitialReservedSize, bool aUseSafeMode = true);
		void ReInit(SizeType aNewSize, bool aUseSafeMode = true);
		inline Type& operator[](const SizeType& aIndex);
		inline const Type& operator[](const SizeType& aIndex) const;

		inline Iterator begin() { return Iterator(myData); };
		inline Iterator end() { return Iterator(myData + myCurrentSize); };
		inline Iterator rbegin() { return Iterator(myData + myCurrentSize - 1); };
		inline Iterator rend() { return Iterator(myData - 1); };

		inline const Iterator begin() const { return Iterator(myData); };
		inline const Iterator end() const { return Iterator(myData + myCurrentSize); };
		inline const Iterator rbegin() const { return Iterator(myData + myCurrentSize - 1); };
		inline const Iterator rend() const { return Iterator(myData - 1); };

		ReverseAdapter Reversed() { return ReverseAdapter(this); };

		inline void Add();
		inline void Add(const Type& aObject);
		inline void Add(const Type&& aObject);
		inline Type Pop();
		inline void Insert(SizeType aIndex, const Type& aObject);
		inline bool DeleteCyclic(Type& aObject, bool aIsArray = false);
		inline void DeleteCyclicAtIndex(SizeType aItemNumber, bool aIsArray = false);
		inline void DeleteAtIndex(SizeType aItemNumber, bool aIsArray = false);
		inline void RemoveAtIndex(SizeType aItemNumber);
		inline bool RemoveCyclic(const Type& aObject);
		inline void RemoveCyclicAtIndex(SizeType aItemNumber);
		inline int Find(const Type& aObject);
		inline Type& GetLast();
		inline const Type& GetLast() const;
		static const int FoundNone = -1;
		inline void RemoveAll();
		inline void DeleteAll(bool aIsArray = false);
		void Optimize();
		__forceinline SizeType Size() const;
		__forceinline SizeType ReservedSize() const;
		inline void Reserve(SizeType aNewSize);
		inline void Resize(SizeType aNewSize);

		inline Type* data();
	private:
		inline void CallAllDestructors() const;
		inline void InternalReserve(SizeType aNewSize);
		Type* myData;
		SizeType myCurrentSize;
		SizeType myAllocatedSize;
		bool mySafeMode;
	};

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>::GrowingArray() :
		myCurrentSize (0),
		myAllocatedSize (0),
		mySafeMode (false),
		myData (nullptr)
	{

	}

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>::GrowingArray(SizeType aNrOfRecommendedItems, bool aUseSafeMode) :
		myCurrentSize(0),
		myAllocatedSize(0),
		mySafeMode(aUseSafeMode),
		myData(nullptr)
	{
		Init(aNrOfRecommendedItems, aUseSafeMode);
	}

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>::GrowingArray(const GrowingArray& aGrowingArray) :
		myCurrentSize(0),
		myAllocatedSize(0),
		mySafeMode(aGrowingArray.mySafeMode),
		myData(nullptr)
	{
		operator=(aGrowingArray);
	}

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>::GrowingArray(GrowingArray&& aGrowingArray) :
		myCurrentSize(0),
		myAllocatedSize(0),
		mySafeMode(aGrowingArray.mySafeMode),
		myData(nullptr)
	{
		operator=(std::move(aGrowingArray));
	}

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>::GrowingArray(const std::initializer_list<Type>& aInitList, bool aUseSafeMode) :
		myCurrentSize(0),
		myAllocatedSize(0),
		mySafeMode(aUseSafeMode),
		myData(nullptr)
	{
		if (aInitList.size() > 0)
		{
			Reserve(static_cast<SizeType>(aInitList.size()));

			if (mySafeMode)
			{
				int index = 0;
				for (auto& iterator : aInitList)
				{
					std::memset(myData + index, 0, sizeof(Type));
					new (myData + index) Type();
					myData[index] = iterator;
					++index;
				}
			}
			else
			{
				memcpy(myData, aInitList.begin(), aInitList.size() * sizeof(Type));
			}

			myCurrentSize = static_cast<SizeType>(aInitList.size());
		}
	}

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>::~GrowingArray()
	{
		if (std::is_class<Type>::value)
		{
			CallAllDestructors();
		}
		myCurrentSize = 0;
		myAllocatedSize = 0;
		delete (void*)myData;
		myData = nullptr;
	}

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>& GrowingArray<Type, SizeType>::operator=(const GrowingArray& aGrowingArray)
	{
		assert(aGrowingArray.myAllocatedSize > 0);

		if (this != &aGrowingArray)
		{
			if (aGrowingArray.myAllocatedSize <= myAllocatedSize)
			{
				if (aGrowingArray.myCurrentSize <= myCurrentSize)
				{
					if (mySafeMode)
					{
						for (int i = 0; i < aGrowingArray.myCurrentSize; ++i)
						{
							myData[i] = aGrowingArray.myData[i];
						}
					}
					else
					{
						memcpy(myData, aGrowingArray.myData, sizeof(Type) * aGrowingArray.myCurrentSize);
					}

					for (int i = aGrowingArray.myCurrentSize; i < myCurrentSize; ++i)
					{
						aGrowingArray.myData[i].~Type();
					}
				}
				else
				{
					if (mySafeMode)
					{
						for (int i = 0; i < myCurrentSize; ++i)
						{
							myData[i] = aGrowingArray.myData[i];
						}
					}
					else
					{
						memcpy(myData, aGrowingArray.myData, sizeof(Type) * myCurrentSize);
					}

					for (int i = myCurrentSize; i < aGrowingArray.myCurrentSize; ++i)
					{
						new (myData + i) Type(aGrowingArray.myData[i]);
					}
				}
			}
			else
			{
				if (std::is_class<Type>::value)
				{
					CallAllDestructors();
				}

				Type* ptr = (Type*) ::operator new (sizeof(Type) * aGrowingArray.myAllocatedSize);

				if (mySafeMode)
				{
					for (int i = 0; i < aGrowingArray.myCurrentSize; ++i)
					{
						new (ptr + i) Type(aGrowingArray.myData[i]);
					}
				}
				else
				{
					memcpy(ptr, aGrowingArray.myData, sizeof(Type) * aGrowingArray.myCurrentSize);
				}

				delete (void*)myData;
				myData = ptr;
				myAllocatedSize = aGrowingArray.myAllocatedSize;
			}

			myCurrentSize = aGrowingArray.myCurrentSize;

		}

		return *this;
	}

	template<typename Type, typename SizeType>
	inline GrowingArray<Type, SizeType>& GrowingArray<Type, SizeType>::operator=(GrowingArray&& aGrowingArray)
	{
		assert(aGrowingArray.myAllocatedSize > 0);

		if (this != &aGrowingArray)
		{

			if (myData != nullptr)
			{
				if (std::is_class<Type>::value)
				{
					CallAllDestructors();
				}
				delete (void*)myData;
			}

			myData = aGrowingArray.myData;
			aGrowingArray.myData = nullptr;
			myCurrentSize = aGrowingArray.myCurrentSize;
			aGrowingArray.myCurrentSize = 0;
			myAllocatedSize = aGrowingArray.myAllocatedSize;
			aGrowingArray.myAllocatedSize = 0;
		}
		return *this;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Init(SizeType aInitialReservedSize, bool aUseSafeMode)
	{
		assert(myAllocatedSize == 0);
		assert(aInitialReservedSize > 0);

		mySafeMode = aUseSafeMode;

		Reserve(aInitialReservedSize);

	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::ReInit(SizeType aNewSize, bool aUseSafeMode)
	{
		assert(myAllocatedSize > 0);
		assert(aNewSize > 0);

		mySafeMode = aUseSafeMode;
		if (std::is_class<Type>::value)
		{
			CallAllDestructors();
		}

		myCurrentSize = 0;
		myAllocatedSize = 0;

		Reserve(aNewSize);

	}

	template<typename Type, typename SizeType>
	inline Type& GrowingArray<Type, SizeType>::operator[](const SizeType& aIndex)
	{
		assert(aIndex >= 0 && aIndex < myCurrentSize);
		return myData[aIndex];
	}

	template<typename Type, typename SizeType>
	inline const Type& GrowingArray<Type, SizeType>::operator[](const SizeType& aIndex) const
	{
		assert(aIndex >= 0 && aIndex < myCurrentSize);
		return myData[aIndex];
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Add()
	{
		assert(myAllocatedSize > 0);

		if (myCurrentSize == myAllocatedSize)
		{
			Reserve(myAllocatedSize * 2);
		}

		new (myData + myCurrentSize) Type();
		++myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Add(const Type& aObject)
	{
		assert(myAllocatedSize > 0);

		if (myCurrentSize == myAllocatedSize)
		{
			Reserve(myAllocatedSize * 2);
		}

		new (myData + myCurrentSize) Type(aObject);
		++myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Add(const Type&& aObject)
	{
		assert(myAllocatedSize > 0);

		if (myCurrentSize == myAllocatedSize)
		{
			Reserve(myAllocatedSize * 2);
		}

		new (myData + myCurrentSize) Type(std::move(aObject));
		++myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Insert(SizeType aIndex, const Type& aObject)
	{
		assert(myAllocatedSize > 0);
		assert((aIndex >= 0 && aIndex < myCurrentSize) || aIndex == myCurrentSize);

		if (myCurrentSize == myAllocatedSize)
		{
			Reserve(myAllocatedSize * 2);
		}
		
		if (mySafeMode)
		{
			for (int i = myCurrentSize; i > aIndex; --i)
			{
				myData[i] = myData[i - 1];
			}
		}
		else
		{
			memmove(myData + (aIndex + 1), myData + aIndex, (myCurrentSize - aIndex) * sizeof(Type));
		}

		myData[aIndex] = aObject;
		++myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline bool GrowingArray<Type, SizeType>::DeleteCyclic(Type& aObject, bool aIsArray)
	{
		assert(myAllocatedSize > 0);
		assert(std::is_pointer<Type>::value);

		for (SizeType index = 0; index < myCurrentSize; ++index)
		{
			if(myData[index] == aObject)
			{
				DeleteCyclicAtIndex(index);

				return true;
			}
		}

		return false;
	}

	template<typename Type, typename SizeType>
	inline bool GrowingArray<Type, SizeType>::RemoveCyclic(const Type& aObject)
	{
		assert(myAllocatedSize > 0);

		for (SizeType index = 0; index < myCurrentSize; ++index)
		{
			if(myData[index] == aObject)
			{
				RemoveCyclicAtIndex(index);

				return true;
			}
		}

		return false;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::DeleteCyclicAtIndex(SizeType aItemNumber, bool aIsArray)
	{
		assert(myAllocatedSize > 0);
		assert(std::is_pointer<Type>::value);
		assert(aItemNumber >= 0 && aItemNumber < myCurrentSize);

		if (aIsArray)
		{
			delete[] myData[aItemNumber];
			myData[aItemNumber] = nullptr;
		}
		else
		{
			delete myData[aItemNumber];
			myData[aItemNumber] = nullptr;
		}

		if (aItemNumber < myCurrentSize - 1)
		{
			memcpy(myData + aItemNumber, myData + (myCurrentSize - 1), sizeof(Type));
		}

		--myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::DeleteAtIndex(SizeType aItemNumber, bool aIsArray)
	{
		assert(myAllocatedSize > 0);
		assert(std::is_pointer<Type>::value);
		assert(aItemNumber >= 0 && aItemNumber < myCurrentSize);

		if (aIsArray)
		{
			delete[] myData[aItemNumber];
			myData[aItemNumber] = nullptr;
		}
		else
		{
			delete myData[aItemNumber];
			myData[aItemNumber] = nullptr;
		}

		if (aItemNumber < myCurrentSize - 1)
		{
			memmove(myData + aItemNumber, myData + aItemNumber + 1, sizeof(Type) * (myCurrentSize - (aItemNumber + 1)));
		}

		--myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::RemoveAtIndex(SizeType aItemNumber)
	{
		assert(myAllocatedSize > 0);
		assert(aItemNumber >= 0 && aItemNumber < myCurrentSize);

		if (std::is_class<Type>::value)
		{
			myData[aItemNumber].~Type();
		}

		if (aItemNumber < myCurrentSize - 1)
		{
			if (mySafeMode)
			{
				for (int i = myCurrentSize - 1; i >= aItemNumber; --i)
				{
					myData[i - 1] = myData[i];
				}
			}
			else
			{
				memmove(myData + aItemNumber, myData + aItemNumber + 1, sizeof(Type) * (myCurrentSize - (aItemNumber + 1)));
			}
		}

		--myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::RemoveCyclicAtIndex(SizeType aItemNumber)
	{
		assert(myAllocatedSize > 0);
		assert(aItemNumber >= 0 && aItemNumber < myCurrentSize);

		if (std::is_class<Type>::value)
		{
			myData[aItemNumber].~Type();
		}

		if (aItemNumber < myCurrentSize - static_cast<SizeType>(1))
		{
			if (mySafeMode)
			{
				myData[aItemNumber] = myData[myCurrentSize - static_cast<SizeType>(1)];
			}
			else
			{
				memcpy(myData + aItemNumber, myData + (myCurrentSize - static_cast<SizeType>(1)), sizeof(Type));
			}
		}

		--myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline Type GrowingArray<Type, SizeType>::Pop()
	{
		assert(myAllocatedSize > 0);
		assert(myCurrentSize > 0);

		--myCurrentSize;

		return std::move(myData[myCurrentSize]);
	}

	template<typename Type, typename SizeType>
	inline int GrowingArray<Type, SizeType>::Find(const Type& aObject)
	{
		assert(myAllocatedSize > 0);

		for (SizeType index = 0; index < myCurrentSize; ++index)
		{
			if (myData[index] == aObject)
			{
				return index;
			}
		}

		return FoundNone;
	}

	template<typename Type, typename SizeType>
	inline Type& GrowingArray<Type, SizeType>::GetLast()
	{
		assert(myAllocatedSize > 0);
		assert(myCurrentSize > 0);

		return myData[myCurrentSize - 1];
	}

	template<typename Type, typename SizeType>
	inline const Type& GrowingArray<Type, SizeType>::GetLast() const
	{
		assert(myAllocatedSize > 0);
		assert(myCurrentSize > 0);

		return myData[myCurrentSize - 1];
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::CallAllDestructors() const
	{
		assert(std::is_class<Type>::value);

		for (SizeType index = myCurrentSize - 1; index >= 0; --index)
		{
			myData[index].~Type();
		}
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::RemoveAll()
	{
		assert(myAllocatedSize > 0);

		if (std::is_class<Type>::value)
		{
			CallAllDestructors();
		}

		myCurrentSize = 0;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::DeleteAll(bool aIsArray)
	{
		assert(myAllocatedSize > 0);
		assert(std::is_pointer<Type>::value);

		if (aIsArray)
		{
			for (SizeType index = 0; index < myCurrentSize; ++index)
			{
				delete[] myData[index];
				myData[index] = nullptr;
			}
		}
		else
		{
			for (SizeType index = 0; index < myCurrentSize; ++index)
			{
				delete myData[index];
				myData[index] = nullptr;
			}
		}

		myCurrentSize = 0;

	}

	template<typename Type, typename SizeType>
	inline SizeType GrowingArray<Type, SizeType>::Size() const
	{
		return myCurrentSize;
	}

	template<typename Type, typename SizeType>
	inline SizeType GrowingArray<Type, SizeType>::ReservedSize() const
	{
		return myAllocatedSize;
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Optimize()
	{
		assert(myAllocatedSize > 0);
		assert(myCurrentSize > 0);

		if (myCurrentSize < myAllocatedSize)
		{
			InternalReserve(myCurrentSize);
		}
	}
	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Reserve(SizeType aNewSize)
	{
		if (aNewSize > myAllocatedSize)
		{
			InternalReserve(aNewSize);
		}
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::Resize(SizeType aNewSize)
	{
		assert(myAllocatedSize > 0);
		assert(aNewSize >= 0);

		if (aNewSize < myCurrentSize)
		{
			if (mySafeMode)
			{
				for (int i = aNewSize; i < myCurrentSize; ++i)
				{
					myData[i].~Type();
				}
			}
			myCurrentSize = aNewSize;
		}
		else if(aNewSize > myCurrentSize)
		{
			if (aNewSize > myAllocatedSize)
			{
				InternalReserve(aNewSize);
			}
			
			for (int index = myCurrentSize; index < aNewSize; ++index)
			{
				std::memset(myData + index, 0, sizeof(Type));
				new (myData + index) Type();
			}

			myCurrentSize = aNewSize;
		}
	}

	template<typename Type, typename SizeType>
	inline void GrowingArray<Type, SizeType>::InternalReserve(SizeType aNewSize)
	{
		Type* ptr = (Type*) ::operator new (sizeof(Type) * aNewSize);

		if (mySafeMode)
		{
			for (int i = 0; i < myCurrentSize; ++i)
			{
				new (ptr + i) Type(myData[i]);
				myData[i].~Type();
			}
		}
		else
		{
			memcpy(ptr, myData, sizeof(Type) * myCurrentSize);
		}

		delete (void*)myData;
		myData = ptr;
		myAllocatedSize = aNewSize;

		//std::memset(myData + myCurrentSize, 0, sizeof(Type) * (myAllocatedSize - myCurrentSize));

	}

	template<typename Type, typename SizeType>
	inline Type* GrowingArray<Type, SizeType>::data()
	{
		return myData;
	}
}