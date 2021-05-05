#pragma once

namespace CommonUtilities
{
	template<typename T>
	class BinaryTree;

	template<typename T>
	class BinaryTreeNode
	{
		friend class BinaryTree<T>;
	public:
		BinaryTreeNode(const BinaryTreeNode<T>& aCopy) = delete;
		BinaryTreeNode<T>& operator=(const BinaryTreeNode<T>& aCopy) = delete;

		inline const T& GetData() const;
		inline T& GetData();

		inline BinaryTreeNode<T>* GetLeft() const;
		inline BinaryTreeNode<T>* GetRight() const;

	private:
		BinaryTreeNode(const T& aValue);
		~BinaryTreeNode() { myData = 0; myLeft = nullptr; myRight = nullptr; };

		T myData;
		BinaryTreeNode<T>* myLeft;
		BinaryTreeNode<T>* myRight;
	};

	template<typename T>
	inline BinaryTreeNode<T>::BinaryTreeNode(const T & aValue) :
		myData(aValue),
		myLeft(nullptr),
		myRight(nullptr)
	{

	}

	template<typename T>
	inline const T& BinaryTreeNode<T>::GetData() const
	{
		return myData;
	}

	template<typename T>
	inline T& BinaryTreeNode<T>::GetData()
	{
		return myData;
	}

	template<typename T>
	inline BinaryTreeNode<T>* BinaryTreeNode<T>::GetLeft() const
	{
		return myLeft;
	}

	template<typename T>
	inline BinaryTreeNode<T>* BinaryTreeNode<T>::GetRight() const
	{
		return myRight;
	}

}