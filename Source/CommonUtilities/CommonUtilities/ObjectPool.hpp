#pragma once

#include <inttypes.h>
#include <cassert>

namespace CommonUtilities
{
	class DisposeAcceptor
	{
	public:
		virtual void Dispose(void* aDisposable) = 0;
	};

	template <typename Type>
	class ObjectPool : public DisposeAcceptor
	{
	private:
		typedef uintptr_t address;

		struct Data
		{
			Data() :
			isFree(true),
			nextPtr(nullptr)
			{};

			Type data;
			bool isFree;
			Data* nextPtr;
		};
	public:
		ObjectPool();
		~ObjectPool();
		void Init(size_t aSize);
		void Release();

		void Dispose(void* aDisposable) override final;

		class Iterator
		{
		public:
			Iterator(Data* aPointer) : myPointer(aPointer) {}
			Iterator operator++() { myPointer++; return *this; }
			bool operator!=(const Iterator & other) const { return myPointer != other.myPointer; }
			Type& operator*() const { return myPointer->data; }
		private:
			Data* myPointer;
		};

		class ReverseIterator
		{
		public:
			ReverseIterator(Data* aPointer) : myPointer(aPointer) {}
			ReverseIterator operator++() { myPointer--; return *this; }
			bool operator!=(const ReverseIterator& other) const { return myPointer != other.myPointer; }
			Type& operator*() { return myPointer->data; }
		private:
			Data* myPointer;
		};

		class ReverseIteratorAdapter
		{
		public:
			ReverseIteratorAdapter(ObjectPool* aPool) : myPool(aPool) {}
			inline ReverseIterator begin() { return myPool->rbegin(); }
			inline ReverseIterator end() { return myPool->rend(); }

		private:
			ObjectPool* myPool;
		};

		inline Iterator begin() { return Iterator(myData); };
		inline Iterator end() {	return Iterator(myData + mySize); };
		ReverseIteratorAdapter Reverse() { return ReverseIteratorAdapter(this); };
		inline ReverseIterator rbegin() { return ReverseIterator(myData + mySize - 1); };
		inline ReverseIterator rend() { return ReverseIterator(myData - 1); };

		const int& Size() const;

		Type& operator[](int aIndex);
		const Type& operator[](int aIndex) const;

		Type* Retrieve();
		void DisposeObject(Type* aObject);

	protected:

	private:

		size_t mySize;
		Data* myFreeSlot;
		Data* myActiveSlot;
		Data* myData;
	};

	template <typename Type>
	ObjectPool<Type>::ObjectPool() :
		myFreeSlot(nullptr),
		myActiveSlot(nullptr),
		myData(nullptr),
		mySize(0)
	{
	}

	template<typename Type>
	inline ObjectPool<Type>::~ObjectPool()
	{
		Release();
	}

	template<typename Type>
	inline const int& ObjectPool<Type>::Size() const
	{
		return mySize;
	}

	template<typename Type>
	inline void ObjectPool<Type>::Init(size_t aSize)
	{
		mySize = aSize;
		myData = new Data[aSize];
		myFreeSlot = myData;

		for (int index = 0; index < mySize - 1; ++index)
		{
			myData[index].nextPtr = &myData[index + 1];
			myData[index].isFree = true;
		}
		myData[mySize - 1].nextPtr = nullptr;
	}
	template<typename Type>
	inline void ObjectPool<Type>::Release()
	{
		if (myData)
		{
			delete[] myData;
			myData = nullptr;
		} 

	}
	template<typename Type>
	inline Type & ObjectPool<Type>::operator[](int aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize);

		return myData[aIndex].data;
	}

	template<typename Type>
	inline const Type & ObjectPool<Type>::operator[](int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < mySize);

		return myData[aIndex].data;
	}

	template <typename Type>
	Type* ObjectPool<Type>::Retrieve()
	{
		if (myFreeSlot != nullptr)
		{
			Data* ptr = myFreeSlot;
			myFreeSlot->isFree = false;
			myFreeSlot = ptr->nextPtr;
			return &ptr->data;
		}
		return nullptr;
	}

	template <typename Type>
	void ObjectPool<Type>::DisposeObject(Type* aObject)
	{
		assert((address)aObject >= (address)myData && (address)aObject < (address)myData + (address)(mySize * sizeof(Data)));

		int index = static_cast<int>((address)aObject - (address)myData) / sizeof(Data);

		if (!myData[index].isFree)
		{
			myData[index].isFree = true;
			myData[index].nextPtr = myFreeSlot;
			myFreeSlot = &myData[index];
		}
	}

	template<typename Type>
	inline void ObjectPool<Type>::Dispose(void* aDisposable)
	{
		ObjectPool<Type>::DisposeObject(reinterpret_cast<Type*>(aDisposable));
	}
}