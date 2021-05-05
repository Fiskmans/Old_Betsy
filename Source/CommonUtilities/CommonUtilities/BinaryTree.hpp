#pragma once
#include "BinaryTreeNode.hpp"
#include "Queue.hpp"
#include <math.h>

namespace CommonUtilities
{
	template <class T>
	class BinaryTree
	{
	public:
		BinaryTree();
		~BinaryTree();

		//Returnerar true om elementet finns i mängden, och false annars.
		bool HasElement(const T& aValue) const;

		//Stoppar in elementet i mängden, om det inte redan finns där. Gör 
		//ingenting annars.
		void Insert(const T& aValue);

		//Plockar bort elementet ur mängden, om det finns. Gör ingenting annars.
		void Remove(const T& aValue);

		const BinaryTreeNode<T> *GetRoot() const;
		void DSWBalance();

		bool IsEmpty() const;
	private:
		BinaryTreeNode<T>* Search(BinaryTreeNode<T>* aNode, const T& aValue) const;
		void DeleteAllChildren(BinaryTreeNode<T>* aNode);
		void DeleteByMerging(BinaryTreeNode<T>*& aNode);
		void RotateRight(BinaryTreeNode<T>* aGrandParent, BinaryTreeNode<T>* aParent, BinaryTreeNode<T>* aChild);
		void RotateLeft(BinaryTreeNode<T>* aGrandParent, BinaryTreeNode<T>* aParent, BinaryTreeNode<T>* aChild);

		BinaryTreeNode<T>* myRoot;
	};

	template<class T>
	inline BinaryTree<T>::BinaryTree() :
		myRoot(nullptr)
	{
	}

	template<class T>
	inline BinaryTree<T>::~BinaryTree()
	{
		if (myRoot != nullptr)
		{
			Queue<BinaryTreeNode<T>*> q;
			q.ReInit(2);
			q.Enqueue(myRoot);
			while (q.GetSize() > 0)
			{
				BinaryTreeNode<T>* node = q.Dequeue();

				if (node->myLeft != nullptr)
				{
					q.Enqueue(node->myLeft);
				}
				if (node->myRight != nullptr)
				{
					q.Enqueue(node->myRight);
				}

				delete node;				
			}
		}

		myRoot = nullptr;
	}

	template<class T>
	inline bool BinaryTree<T>::HasElement(const T& aValue) const
	{
		return Search(myRoot, aValue) != nullptr;
	}

	template<class T>
	inline void BinaryTree<T>::Insert(const T& aValue)
	{
		BinaryTreeNode<T>* node = myRoot;
		BinaryTreeNode<T>* prev = nullptr;

		while (node != nullptr)
		{
			// find a place for inserting new node;
			prev = node;
			if (aValue < node->myData)
				node = node->myLeft;
			else node = node->myRight;
		}

		if (myRoot == nullptr)
		{
			myRoot = new BinaryTreeNode<T>(aValue);
		} 
		else if (aValue < prev->myData)
		{
			prev->myLeft = new BinaryTreeNode<T>(aValue);
		}
		else
		{
			prev->myRight = new BinaryTreeNode<T>(aValue);
		}
	}

	template<class T>
	inline void BinaryTree<T>::Remove(const T& aValue)
	{
		BinaryTreeNode<T> *node = myRoot, *prev = 0;
		while (node != nullptr) 
		{ 
			if (node->myData == aValue)
			{
				break;
			}
			prev = node;        
			if (aValue < node->myData)
			{
				node = node->myLeft;
			}
			else
			{
				node = node->myRight; 
			}
		}
		if (node != nullptr && node->myData == aValue)
		{
			if (node == myRoot)
			{
				DeleteByMerging(myRoot);
			}
			else if (prev->myLeft == node)
			{
				DeleteByMerging(prev->myLeft);
			}
			else 
			{ 
				DeleteByMerging(prev->myRight); 
			}
		}

	}

	template<class T>
	inline const BinaryTreeNode<T>* BinaryTree<T>::GetRoot() const
	{
		return myRoot;
	}

	template<class T>
	inline void BinaryTree<T>::DSWBalance()
	{
		//Get a backbone through right rotation

		int n = 0;

		BinaryTreeNode<T>* prev = nullptr;
		BinaryTreeNode<T>* tmp = myRoot;
		while (tmp != nullptr)
		{
			if (tmp->myLeft != nullptr)
			{
				BinaryTreeNode<T>* leftChild = tmp->myLeft;
				RotateRight(prev, tmp, leftChild);
				if (prev)
				{
					tmp = prev->myRight;
				}
				else
				{
					myRoot = leftChild;
					tmp = leftChild;
				}
			}
			else
			{
				prev = tmp;
				tmp = tmp->myRight;
				++n;
			}
		}

		//Balance through left rotation

		double double_m = std::pow(2,std::floorf(std::log2(n + 1))) - 1;
		int m = double_m;
		int preLoopCount = n - m;
		//Pre balance m - n times
		prev = nullptr;
		tmp = myRoot;
		for (int i = 0; i < preLoopCount; ++i)
		{
			BinaryTreeNode<T>* rightChild = tmp->myRight;
			RotateLeft(prev, tmp, rightChild);
			if (prev)
			{
				tmp = rightChild->myRight;
				prev = rightChild;
			}
			else
			{
				myRoot = rightChild;
				prev = rightChild;
				tmp = rightChild->myRight;
			}
		}

		//Rotate the tree
		while (m > 1)
		{
			m = m / 2;
			prev = nullptr;
			tmp = myRoot;
			for (int i = 0; i < m; ++i)
			{
				BinaryTreeNode<T>* rightChild = tmp->myRight;
				RotateLeft(prev, tmp, rightChild);
				if (prev)
				{
					tmp = rightChild->myRight;
					prev = rightChild;
				}
				else
				{
					myRoot = rightChild;
					prev = rightChild;
					tmp = rightChild->myRight;
				}
			}
		}

	}

	template<class T>
	inline bool BinaryTree<T>::IsEmpty() const
	{
		return myRoot == nullptr;
	}

	template<class T>
	inline BinaryTreeNode<T>* BinaryTree<T>::Search(BinaryTreeNode<T>* aNode, const T& aValue) const
	{
		while (aNode != nullptr)
		{
			if (aValue == aNode->myData)
			{
				return aNode;
			}
			else if (aValue < aNode->myData)
			{
				aNode = aNode->myLeft;
			}
			else
			{
				aNode = aNode->myRight;
			}
		}
		return nullptr;
	}

	template<class T>
	inline void BinaryTree<T>::DeleteAllChildren(BinaryTreeNode<T>* aNode)
	{
		if (aNode != nullptr)
		{
			DeleteAllChildren(aNode->myRight);
			aNode->myRight = nullptr;
			DeleteAllChildren(aNode->myLeft);
			aNode->myLeft = nullptr;
			delete this;
		}
	}
	template<class T>
	inline void BinaryTree<T>::DeleteByMerging(BinaryTreeNode<T>*& aNode)
	{
		BinaryTreeNode<T>* tmp = aNode;
		if (aNode != nullptr)
		{
			if (aNode->myRight == nullptr)
			{
				aNode = aNode->myLeft;
			}
			else if (aNode->myLeft == nullptr)
			{
				aNode = aNode->myRight;
			}
			else
			{
				tmp = aNode->myLeft;
				while (tmp->myRight != nullptr)
				{
					tmp = tmp->myRight;
				}
				tmp->myRight = aNode->myRight;
				tmp = aNode;
				aNode = aNode->myLeft;
			}
			delete tmp;
			if (tmp == myRoot)
			{
				myRoot = nullptr;
			}
			tmp = nullptr;
		}
	}

	template<class T>
	inline void BinaryTree<T>::RotateRight(BinaryTreeNode<T>* aGrandParent, BinaryTreeNode<T>* aParent, BinaryTreeNode<T>* aChild)
	{
		if (aGrandParent != nullptr)
		{
			aGrandParent->myRight = aChild;
		}
		aParent->myLeft = aChild->myRight;
		aChild->myRight = aParent;
	}

	template<class T>
	inline void BinaryTree<T>::RotateLeft(BinaryTreeNode<T>* aGrandParent, BinaryTreeNode<T>* aParent, BinaryTreeNode<T>* aChild)
	{
		if (aGrandParent != nullptr)
		{
			aGrandParent->myRight = aChild;
		}
		aParent->myRight = aChild->myLeft;
		aChild->myLeft = aParent;
	}
}