#pragma once


template<class T, size_t SlotCount = 1>
class Handover
{
public:
	Handover();

	void Add(const T& aData);
	bool AddFailable(const T& aData);

	bool Get(T& aOutData);

private:
	struct Slot
	{
		std::atomic<bool> myEmpty;
		T myData;
	};

	Slot mySlots[SlotCount];
};

template<class T, size_t SlotCount>
inline Handover<T, SlotCount>::Handover()
{
	for (size_t i = 0; i < SlotCount; i++)
	{
		mySlots[i].myEmpty = true;
	}
}

template<class T, size_t SlotCount>
inline void Handover<T, SlotCount>::Add(const T& aData)
{
	while (!AddFailable(aData))
	{
		std::this_thread::yield();
	}
}

template<class T, size_t SlotCount>
inline bool Handover<T, SlotCount>::AddFailable(const T& aData)
{
	for (size_t i = 0; i < SlotCount; i++)
	{
		if (mySlots[i].myEmpty.load())
		{
			mySlots[i].myData = aData;
			mySlots[i].myEmpty = false;
			return true;
		}
	}
	return false;
}

template<class T, size_t SlotCount>
inline bool Handover<T, SlotCount>::Get(T& aOutData)
{
	for (size_t i = 0; i < SlotCount; i++)
	{
		if (!mySlots[i].myEmpty.load())
		{
			aOutData = mySlots[i].myData;
			mySlots[i].myEmpty = true;
			return true;
		}
	}
	return false;
}
