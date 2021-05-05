#pragma once
#include "pch.h"

template <class T>
class GridNode
{
public:
	GridNode() = default;
	~GridNode() = default;

	void Add(const T* anElement, const bool aCheckIfAdded = false);
	void Remove(const T& anElement);
	void Clear();

	std::vector<T>& GetData();
	const std::vector<T>& GetData() const;

private:
	std::vector<T> myData;
};

template <class T>
class Grid
{

public:
	Grid() = default;
	~Grid() = default;

	void Init(const V2F& aMin, const V2F& aMax, const float aApproxNodeSize);

	void AddAtPos(const T& anElement, const V2F& aPosition, const bool aCheckIfAdded = false);

	GridNode<T>& GetNode(const V2F& aPosition);
	const GridNode<T>& GetNode(const V2F& aPosition) const;

	GridNode<T>& operator[](const int anIndex);
	const GridNode<T>& operator[](const int anIndex) const;

	GridNode<T>& operator()(const int aX, const int aY);
	const GridNode<T>& operator()(const int aX, const int aY) const;

	CU::Vector2<int> GetGridIndex(const V2F& aPosition) const;

	size_t GetWidth() const;
	size_t GetHeight() const;
	size_t GetNumberOfNodes() const;

	const V2F& GetNodeSize() const;
	const V2F& GetCenterPosition() const;


private:
	std::vector<std::vector<GridNode<T>>> myNodes;

	size_t myWidth = 0;
	size_t myHeight = 0;
	V2F myNodeSize;
	V2F myCenterPosition;
	V2F myMin;
};

template<class T>
inline void Grid<T>::Init(const V2F& aMin, const V2F& aMax, const float aApproxNodeSize)
{
	if (aMin.x > aMax.x || aMin.y > aMax.y)
	{
		SYSWARNING("Min was bigger than max in Grid::Init","");
	}

	const float width = aMax.x - aMin.x;
	const float height = aMax.y - aMin.y;

	myCenterPosition = V2F(aMin.x + width * 0.5f, aMin.y + height * 0.5f);

	myMin = aMin;

	myWidth = CAST(size_t, floor(width / aApproxNodeSize));
	myHeight = CAST(size_t, floor(height / aApproxNodeSize));

	myNodeSize.x = width / myWidth;
	myNodeSize.y = height / myHeight;

	myNodes.clear();

	myNodes.resize(myWidth);

	for (auto& list : myNodes)
	{
		list.resize(myHeight);
	}

}

template<class T>
inline void Grid<T>::AddAtPos(const T& anElement, const V2F& aPosition, const bool aCheckIfAdded)
{
	GetNode(aPosition).Add(&anElement, aCheckIfAdded);
}


template<class T>
inline GridNode<T>& Grid<T>::GetNode(const V2F& aPosition)
{
	const CU::Vector2<int> pos = GetGridIndex(aPosition);

	return (*this)(pos.x, pos.y);
}

template<class T>
inline const GridNode<T>& Grid<T>::GetNode(const V2F& aPosition) const
{
	const CU::Vector2<int> pos = GetGridIndex(aPosition);
	return (*this)(pos.x, pos.y);
}

template<class T>
inline GridNode<T>& Grid<T>::operator[](const int anIndex)
{
	size_t x = anIndex / myWidth;
	size_t y = anIndex % myWidth;

	if (x < 0 || x >= myWidth || y < 0 || y >= myHeight)
	{
		ONETIMEWARNING("Gird indexed out of range!","");

		x = CLAMP(0, myWidth -1 , x);
		y = CLAMP(0, myHeight - 1, y);
	}

	return myNodes[x][y];
}

template<class T>
inline const GridNode<T>& Grid<T>::operator[](const int anIndex) const
{
	int x = anIndex / myWidth;
	int y = anIndex % myWidth;

	if (x < 0 || x >= myWidth || y < 0 || y >= myHeight)
	{
		ONETIMEWARNING("Gird indexed out of range!","");

		x = CLAMP(0, myWidth - 1, x);
		y = CLAMP(0, myHeight - 1, y);
	}

	return myNodes[x][y];
}

template<class T>
inline GridNode<T>& Grid<T>::operator()(const int aX, const int aY)
{
	if (aX < 0 || aX >= myWidth || aY < 0 || aY >= myHeight)
	{
		ONETIMEWARNING("Gird indexed out of range!","");

		const size_t x = CLAMP(0, myWidth - 1, aX);
		const size_t y = CLAMP(0, myHeight - 1, aY);

		return myNodes[x][y];
	}

	return myNodes[aX][aY];
}

template<class T>
inline const GridNode<T>& Grid<T>::operator()(const int aX, const int aY) const
{
	if (aX < 0 || aX >= myWidth || aY < 0 || aY >= myHeight)
	{
		ONETIMEWARNING("Gird indexed out of range!","");

		const int x = CLAMP(0, myWidth - 1, aX);
		const int y = CLAMP(0, myHeight - 1, aY);

		return myNodes[x][y];
	}

	return myNodes[aX][aY];
}

template<class T>
inline CU::Vector2<int> Grid<T>::GetGridIndex(const V2F& aPosition) const
{
	return CU::Vector2<int>(CAST(int, floor(aPosition.x / myNodeSize.x)), CAST(int, floor(aPosition.y / myNodeSize.y))) - CU::Vector2<int>(CAST(int, floor(myMin.x / myNodeSize.x)), CAST(int, floor(myMin.y / myNodeSize.y)));
}

template<class T>
inline size_t Grid<T>::GetWidth() const
{
	return myWidth;
}

template<class T>
inline size_t Grid<T>::GetHeight() const
{
	return myHeight;
}

template<class T>
inline size_t Grid<T>::GetNumberOfNodes() const
{
	return myHeight * myWidth;
}

template<class T>
inline const V2F& Grid<T>::GetNodeSize() const
{
	return myNodeSize;
}

template<class T>
inline const V2F& Grid<T>::GetCenterPosition() const
{
	return myCenterPosition;
}


template<class T>
inline void GridNode<T>::Add(const T* anElement, const bool aCheckIfAdded)
{
	if (aCheckIfAdded)
	{
		for (int i = CAST(int, myData.size()) - 1; i >= 0; i--)
		{
			if (&myData[i] == anElement)
			{
				return;
			}
		}
	}

	myData.push_back(*anElement);
}


template<class T>
inline void GridNode<T>::Remove(const T& anElement)
{
	for (int i = myData.size() - 1; i >= 0; i--)
	{
		if (myData[i] == anElement)
		{
			myData.erase(myData.begin() + i);
			return;
		}
	}
}


template<class T>
inline void GridNode<T>::Clear()
{
	myData.clear();
}


template<class T>
inline std::vector<T>& GridNode<T>::GetData()
{
	return myData;
}


template<class T>
inline const std::vector<T>& GridNode<T>::GetData() const
{
	return myData;
}

