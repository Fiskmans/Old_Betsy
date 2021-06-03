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
		// Copy-konstruktorn och assignment-operatorn är borttagna, så att det enda
		// sättet att skapa en nod är genom att stoppa in ett värde i en lista.
		DoublyLinkedListNode(const DoublyLinkedListNode<T>& aCopy) = delete;
		DoublyLinkedListNode<T>& operator=(const DoublyLinkedListNode<T>& aCopy) = delete;

		// Returnerar nodens värde
		const T& GetValue() const;
		T& GetValue();

		// Returnerar nästa nod i listan, eller nullptr om noden är sist i listan
		DoublyLinkedListNode<T>* GetNext() const;

		// Returnerar föregående nod i listan, eller nullptr om noden är först i
		// listan
		DoublyLinkedListNode<T>* GetPrevious() const;

	private:
		// Konstruktorn och destruktorn är privat, så att man inte kan skapa eller
		// ta bort noder utifrån. List-klassen är friend, så att den kan skapa
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