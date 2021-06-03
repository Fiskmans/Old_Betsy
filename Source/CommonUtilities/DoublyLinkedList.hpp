#pragma once

#include <cassert>
#include "DoublyLinkedListNode.hpp"

namespace CommonUtilities
{
	template<typename T>
	class DoublyLinkedList
	{
	public:

		// Skapar en tom lista
		DoublyLinkedList();
		// Frigör allt minne som listan allokerat
		~DoublyLinkedList();

		// Returnerar antalet element i listan
		inline int GetSize() const;

		// Returnerar första noden i listan, eller nullptr om listan är tom
		inline DoublyLinkedListNode<T>* GetFirst();

		// Returnerar sista noden i listan, eller nullptr om listan är tom
		inline DoublyLinkedListNode<T>* GetLast();

		// Skjuter in ett nytt element först i listan
		inline void InsertFirst(const T& aValue);

		// Skjuter in ett nytt element sist i listan
		inline void InsertLast(const T& aValue);

		// Skjuter in ett nytt element innan aNode
		inline void InsertBefore(DoublyLinkedListNode<T>* aNode, const T& aValue);

		// Skjuter in ett nytt element efter aNode
		inline void InsertAfter(DoublyLinkedListNode<T>* aNode, const T& aValue);

		// Plockar bort noden ur listan och frigör minne. (Det är ok att anta att
		// aNode är en nod i listan, och inte från en annan lista)
		inline void Remove(DoublyLinkedListNode<T>* aNode);

		// Hittar första elementet i listan som har ett visst värde. Jämförelsen
		// görs med operator==. Om inget element hittas returneras nullptr.
		inline DoublyLinkedListNode<T>* FindFirst(const T& aValue);

		// Hittar sista elementet i listan som har ett visst värde. Jämförelsen
		// görs med operator==. Om inget element hittas returneras nullptr.
		inline DoublyLinkedListNode<T>* FindLast(const T& aValue);

		// Plockar bort första elementet i listan som har ett visst värde. 
		// Jämförelsen görs med operator==. Om inget element hittas görs ingenting.
		// Returnerar true om ett element plockades bort, och false annars.
		inline bool RemoveFirst(const T& aValue);

		// Plockar bort sista elementet i listan som har ett visst värde.
		// Jämförelsen görs med operator==. Om inget element hittas görs ingenting.
		// Returnerar true om ett element plockades bort, och false annars.
		inline bool RemoveLast(const T& aValue);

	private:
		int mySize;
		DoublyLinkedListNode<T>* myFirstNode;
		DoublyLinkedListNode<T>* myLastNode;
	};


	template<typename T>
	DoublyLinkedList<T>::DoublyLinkedList() :
		mySize(0),
		myFirstNode(nullptr),
		myLastNode(nullptr)
	{
	}

	template<typename T>
	DoublyLinkedList<T>::~DoublyLinkedList()
	{
		DoublyLinkedListNode<T>* iterator = myFirstNode;
		while (iterator != nullptr)
		{
			DoublyLinkedListNode<T>* temp = iterator;
			iterator = iterator->myNext;
			delete temp;
		}
		myFirstNode = nullptr;
		myLastNode = nullptr;
	}
	template<typename T>
	int DoublyLinkedList<T>::GetSize() const
	{
		return mySize;
	}

	template<typename T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetFirst()
	{
		return myFirstNode;
	}

	template<typename T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetLast()
	{
		//DoublyLinkedListNode<T>* iterator = myFirstNode;
		//for (int i = 0; i < mySize - 1; ++i)
		//{
		//	iterator = iterator->GetNext();
		//}
		//return iterator;
		return myLastNode;
	}

	template<typename T>
	void DoublyLinkedList<T>::InsertFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* obj = new DoublyLinkedListNode<T>(aValue);
		if (myFirstNode != nullptr)
		{
			myFirstNode->myPrevious = obj;
			obj->myNext = myFirstNode;
		}
		myFirstNode = obj;
		if (myLastNode == nullptr)
		{
			myLastNode = obj;
		}
		++mySize;

	}

	template<typename T>
	void DoublyLinkedList<T>::InsertLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* obj = new DoublyLinkedListNode<T>(aValue);
		if (myLastNode != nullptr)
		{
			myLastNode->myNext = obj;
			obj->myPrevious = myLastNode;
		}
		myLastNode = obj;
		if (myFirstNode == nullptr)
		{
			myFirstNode = obj;
		}
		++mySize;
	}

	template<typename T>
	void DoublyLinkedList<T>::InsertBefore(DoublyLinkedListNode<T>* aNode, const T& aValue)
	{
		assert(aNode != nullptr && "aNode is nullptr");

		DoublyLinkedListNode<T>* obj = new DoublyLinkedListNode<T>(aValue);
		obj->myPrevious = aNode->myPrevious;
		obj->myNext = aNode;
		if (aNode->myPrevious != nullptr)
		{
			aNode->myPrevious->myNext = obj;
		}
		else
		{
			myFirstNode = obj;
		}
		aNode->myPrevious = obj;
		++mySize;
	}

	template<typename T>
	void DoublyLinkedList<T>::InsertAfter(DoublyLinkedListNode<T>* aNode, const T& aValue)
	{
		assert(aNode != nullptr && "aNode is nullptr");

		DoublyLinkedListNode<T>* obj = new DoublyLinkedListNode<T>(aValue);
		obj->myPrevious = aNode;
		if (aNode->myNext != nullptr)
		{
			obj->myNext = aNode->myNext;
			aNode->myNext->myPrevious = obj;
		}
		else
		{
			myLastNode = obj;
		}
		aNode->myNext = obj;
		++mySize;
	}

	template<typename T>
	void DoublyLinkedList<T>::Remove(DoublyLinkedListNode<T>* aNode)
	{
		assert(aNode != nullptr && "aNode is nullptr");

		if (aNode->myPrevious != nullptr && aNode->myNext != nullptr)
		{
			aNode->myPrevious->myNext = aNode->myNext;
			aNode->myNext->myPrevious = aNode->myPrevious;
		}
		else if (aNode->myPrevious != nullptr)
		{
			aNode->myPrevious->myNext = nullptr;
			myLastNode = aNode->myPrevious;
		}
		else if (aNode->myNext != nullptr)
		{
			aNode->myNext->myPrevious = nullptr;
			myFirstNode = aNode->myNext;
		}

		delete aNode;
		--mySize;

		if (mySize == 0)
		{
			myFirstNode = nullptr;
			myLastNode = nullptr;
		}
	}

	template<typename T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindFirst(const T& aValue)
	{
		if (myFirstNode != nullptr)
		{
			DoublyLinkedListNode<T>* iterator = myFirstNode;
			do
			{
				if (iterator->myData == aValue)
				{
					return iterator;
				}
				iterator = iterator->myNext;
			} while (iterator != nullptr);
		}

		return nullptr;
	}

	template<typename T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindLast(const T& aValue)
	{
		if (myFirstNode != nullptr)
		{
			DoublyLinkedListNode<T>* found = nullptr;
			DoublyLinkedListNode<T>* iterator = myFirstNode;
			do
			{
				if (iterator->myData == aValue)
				{
					found = iterator;
				}
				iterator = iterator->myNext;
			} while (iterator != nullptr);

			return found;
		}

		return nullptr;
	}

	template<typename T>
	bool DoublyLinkedList<T>::RemoveFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* found = FindFirst(aValue);
		if (found != nullptr)
		{
			Remove(found);
			return true;
		}
		return false;
	}

	template<typename T>
	bool DoublyLinkedList<T>::RemoveLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* found = FindLast(aValue);
		if (found != nullptr)
		{
			Remove(found);
			return true;
		}
		return false;
	}

}