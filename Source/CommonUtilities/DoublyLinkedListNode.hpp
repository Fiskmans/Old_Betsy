#pragma once

namespace CommonUtilities
{
	template<typename T>
	class DoublyLinkedList;

	template <typename T>
	class DoublyLinkedListNode
	{
		friend DoublyLinkedList<T>;

	public:
		// Copy-konstruktorn och assignment-operatorn �r borttagna, s� att det enda
		// s�ttet att skapa en nod �r genom att stoppa in ett v�rde i en lista.
		DoublyLinkedListNode(const DoublyLinkedListNode<T>& aCopy) = delete;
		DoublyLinkedListNode<T>& operator=(const DoublyLinkedListNode<T>& aCopy) = delete;

		// Returnerar nodens v�rde
		const T& GetValue() const;
		T& GetValue();

		// Returnerar n�sta nod i listan, eller nullptr om noden �r sist i listan
		DoublyLinkedListNode<T>* GetNext() const;

		// Returnerar f�reg�ende nod i listan, eller nullptr om noden �r f�rst i
		// listan
		DoublyLinkedListNode<T>* GetPrevious() const;

	private:
		// Konstruktorn och destruktorn �r privat, s� att man inte kan skapa eller
		// ta bort noder utifr�n. List-klassen �r friend, s� att den kan skapa
		// eller ta bort noder.
		DoublyLinkedListNode(const T& aValue);
		~DoublyLinkedListNode() {};


		DoublyLinkedListNode<T>* myPrevious;
		DoublyLinkedListNode<T>* myNext;
		T myData;
	};

	template<typename T>
	inline const T& DoublyLinkedListNode<T>::GetValue() const
	{
		return myData;
	}

	template<typename T>
	inline T& DoublyLinkedListNode<T>::GetValue()
	{
		return myData;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetNext() const
	{
		return myNext;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetPrevious() const
	{
		return myPrevious;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>::DoublyLinkedListNode(const T& aValue) :
		myNext(nullptr),
		myPrevious(nullptr),
		myData(aValue)
	{
	}
}