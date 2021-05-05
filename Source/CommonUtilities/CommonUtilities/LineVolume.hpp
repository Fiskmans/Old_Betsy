#pragma once

#include "Line.hpp"
#include <vector>

namespace CommonUtilities
{
	template<typename T>
	class LineVolume
	{
	public:
		LineVolume() = default;
		LineVolume(const std::vector<Line<T>>& aLineList);

		void AddLine(const Line<T>& aLine);

		bool Inside(const Vector2<T>& aPosition);

		int Size() const;
	private:
		std::vector<Line<T>> myData;
	};
	template<typename T>
	inline LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
	{
		myData = aLineList;
	}
	template<typename T>
	inline void LineVolume<T>::AddLine(const Line<T>& aLine)
	{
		myData.push_back(aLine);
	}
	template<typename T>
	inline bool LineVolume<T>::Inside(const Vector2<T>& aPosition)
	{
		for (auto& iterator : myData)
		{
			if (!iterator.Inside(aPosition))
			{
				return false;
			}
		}

		return true;
	}
	template<typename T>
	inline int LineVolume<T>::Size() const
	{
		return myData.size();
	}
}