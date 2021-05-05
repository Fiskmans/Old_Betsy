#pragma once
#include "GrowingArray.hpp"

namespace CommonUtilities
{
	template <class T>
	void SelectionSort(GrowingArray<T> &aVector, bool aSafemode = true);
	template <class T>
	void BubbleSort(GrowingArray<T> &aVector, bool aSafemode = true);
	template <class T>
	void QuickSort(GrowingArray<T> &aVector, bool aSafemode = true);
	template <class T>
	void MergeSort(GrowingArray<T> &aVector, bool aSafemode = true);

	template<class T>
	int InternalPartition(GrowingArray<T>& aVector, int aLow, int aHigh, bool aSafemode = true);
	template<class T>
	void InternalQuickSort(GrowingArray<T>& aVector, int aLeft, int aRight, bool aSafemode = true);
	template<class T>
	void InternalQuickSortIterative(GrowingArray<T>& aVector, int aLow, int aHigh, bool aSafemode = true);
	template<class T>
	void InternalMerge(GrowingArray<T>& aVector, int aLeft, int aMiddle, int aRight);
	template<class T>
	void InternalMergeSort(GrowingArray<T>& aVector, int aLeft, int aRight);

	template<class T>
	inline void SelectionSort(GrowingArray<T>& aVector, bool aSafemode)
	{
		int n = aVector.Size();
		for (int i = 0; i < n - 1; ++i)
		{
			int minIndex = i;
			for (int j = i + 1; j < n; ++j)
			{
				if (aVector[j] < aVector[minIndex])
				{
					minIndex = j;
				}
			}
			std::swap(aVector[minIndex], aVector[i]);
		}
	}

	template<class T>
	inline void BubbleSort(GrowingArray<T>& aVector, bool aSafemode)
	{
		int n = aVector.Size();
		for (int i = 0; i < n - 1; ++i)
		{
			for (int j = 0; j < n - i - 1; ++j)
			{
				if (aVector[j + 1] < aVector[j])
				{
					std::swap(aVector[j], aVector[j + 1]);
				}
			}
		}
	}

	template<class T>
	int InternalPartition(GrowingArray<T>& aVector, int aLow, int aHigh, bool aSafemode)
	{
		T pivot = aVector[aHigh];

		int i = (aLow - 1);

		for (int j = aLow; j < aHigh; ++j)
		{
			if (aVector[j] < pivot)
			{
				++i;
				std::swap(aVector[i], aVector[j]);
			}
		}
		std::swap(aVector[i + 1], aVector[aHigh]);
		return i + 1;
	}

	template<class T>
	void InternalQuickSort(GrowingArray<T>& aVector, int aLow, int aHigh, bool aSafemode)
	{
		if (aLow < aHigh)
		{
			if (aHigh - aLow < 5)
			{
				SelectionSort(aVector);
			}
			else
			{
				int pi = InternalPartition(aVector, aLow, aHigh);

				InternalQuickSort(aVector, aLow, pi - 1);
				InternalQuickSort(aVector, pi + 1, aHigh);
			}
		}
	}

	template<class T>
	void InternalQuickSortIterative(GrowingArray<T>& aVector, int aLow, int aHigh, bool aSafemode)
	{
		if (aLow < aHigh)
		{
			int* stack = new int[aHigh - aLow + 1];
			int top = -1;
			stack[++top] = aLow;
			stack[++top] = aHigh;
			while (top >= 0)
			{
				aHigh = stack[top--];
				aLow = stack[top--];

				int p = InternalPartition(aVector, aLow, aHigh);

				if (p - 1 > aLow)
				{
					stack[++top] = aLow;
					stack[++top] = p - 1;
				}

				if (p + 1 < aHigh)
				{
					stack[++top] = p + 1;
					stack[++top] = aHigh;
				}
			}

			delete stack;
		}
	}

	template<class T>
	inline void QuickSort(GrowingArray<T>& aVector, bool aSafemode)
	{
		InternalQuickSortIterative(aVector, 0, aVector.Size() - 1);
	}

	template<class T>
	void InternalMerge(GrowingArray<T>& aVector, int aLeft, int aMiddle, int aRight)
	{
		int i, j, k;
		int n1 = aMiddle - aLeft + 1;
		int n2 = aRight - aMiddle;

		T* L = new T[n1];
		T* R = new T[n2];

		for (i = 0; i < n1; ++i)
		{
			L[i] = aVector[aLeft + i];
		}
		for (j = 0; j < n2; ++j)
		{
			R[j] = aVector[aMiddle + 1 + j];
		}

		i = 0;
		j = 0;
		k = aLeft;

		while (i < n1 && j < n2)
		{
			if (L[i] < R[j])
			{
				aVector[k] = L[i];
				++i;
			}
			else
			{
				aVector[k] = R[j];
				++j;
			}
			++k;
		}

		while (i < n1)
		{
			aVector[k] = L[i];
			++i;
			++k;
		}

		while (j < n2)
		{
			aVector[k] = R[j];
			++j;
			++k;
		}


		delete L;
		delete R;
	}

	template<class T>
	void InternalMergeSort(GrowingArray<T>& aVector, int aLeft, int aRight)
	{
		if (aLeft < aRight)
		{
			int m = aLeft + (aRight - aLeft) / 2;
			InternalMergeSort(aVector, aLeft, m);
			InternalMergeSort(aVector, m + 1, aRight);

			InternalMerge(aVector, aLeft, m, aRight);
		}
	}

	template<class T>
	inline void MergeSort(GrowingArray<T>& aVector, bool aSafemode)
	{
		InternalMergeSort(aVector, 0, aVector.Size() - 1);
	}
}