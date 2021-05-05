#include <pch.h>
#include "MemoryChunk.h"
#include <thread>
#include <set>

NewFromOS DefaultAllocator;

Allocator* GlobalAllocator = &DefaultAllocator;
std::thread::id AllocatorThread;


void Memory::SetGlobalAllocator(Allocator* aAllocator)
{
	GlobalAllocator = aAllocator;
	AllocatorThread = std::this_thread::get_id();
}

void Memory::DefaultGlobalAllocator()
{
	GlobalAllocator = &DefaultAllocator;
}


#ifdef OHNO
void* operator new(size_t aSize)
{
#ifdef _DEBUG
	if (!GlobalAllocator)
	{
		SYSERROR("Trying to new without assigned allocator");
	}
	if (AllocatorThread != std::thread::id() && std::this_thread::get_id() != AllocatorThread)
	{
		//SYSWARNING("Allocating on another thread than the main one. ");
		return malloc(aSize);
	}
#endif // _DEBUG

	void* result = GlobalAllocator->New(aSize);
	return result;
}

void operator delete(void* aPtr)
{

#ifdef _DEBUG
	if (AllocatorThread != std::thread::id() && std::this_thread::get_id() != AllocatorThread)
	{
		//SYSWARNING("Deleting on another thread than the main one. ");
		free(aPtr);
		return;
	}
#endif // _DEBUG
	GlobalAllocator->Delete(aPtr);

}
#endif // DEBUG






void* NewFromOS::New(size_t size)
{
	return malloc(size);
}

size_t NewFromOS::GetAvailability()
{
	return 0;
}

size_t NewFromOS::GetFragmentation()
{
	return 0;
}

void NewFromOS::Delete(void* aPtr)
{
	free(aPtr);
}





MemoryChunk::MemoryChunk(size_t aSize) : Size(aSize)
{
	myData = malloc(Size);
	mySegments[aSize].push({ myData,Size });
	myAvailable = Size;
}

MemoryChunk::~MemoryChunk()
{
	free(myData);
	myData = nullptr;
}

void* MemoryChunk::Get(size_t size)
{
	static_assert(sizeof(char) == 1, "Yeah... ");
	for (auto& segmentList : mySegments)
	{
		int a = static_cast<int>(segmentList.first);
		if (segmentList.first >= size && !segmentList.second.empty())
		{
			Memory::Segment seg = segmentList.second.top();
			segmentList.second.pop();
			if (segmentList.second.empty())
			{
				mySegments.erase(seg.mySize);
			}

			if (seg.mySize != size)
			{
				mySegments[seg.mySize - size].push({ static_cast<void*>(static_cast<char*>(seg.myAddress) + size),seg.mySize - size });
			}
			myAvailable -= size;
			myInUse[seg.myAddress] = size;
			return seg.myAddress;
		}
	}
	SYSERROR("Out of memory","welp");
	return nullptr;
}

size_t MemoryChunk::GetAvailability()
{
	return myAvailable;
}

size_t MemoryChunk::GetFragmentation()
{
	size_t total = 0;
	for (auto& i : mySegments)
	{
		total += i.second.size();
	}
	return total;
}

void MemoryChunk::Return(void* aPtr)
{
	static_assert(sizeof(char) == 1, "yeah...");
	if (aPtr > myData && aPtr < static_cast<char*>(myData) + Size)
	{
		myAvailable += myInUse[aPtr];
		mySegments[myInUse[aPtr]].push({ aPtr,myInUse[aPtr] });
		myInUse.erase(aPtr);
	}
	else
	{
		SYSERROR("Trying to return memory that is not owned","welp");
	}
}

void MemoryChunk::DeFragment()
{
	static_assert(sizeof(char) == 1, "double check this works if false");
	std::vector<Memory::Segment, RawMallocAllocator<Memory::Segment>> unpacked;
	for (auto& stack : mySegments) //unpack
	{
		Memory::Segment seg;
		while (!stack.second.empty())
		{
			seg = stack.second.top();
			stack.second.pop();
			unpacked.push_back(seg);
		}
	}
	mySegments.clear();

	bool anythingLeft = true;
	while (anythingLeft)
	{
		anythingLeft = false;
		Memory::Segment source;
		Memory::Segment target;
		size_t targetIndex;
		size_t sourceIndex;
		for (sourceIndex = 0; sourceIndex < unpacked.size(); sourceIndex++)
		{
			source = unpacked[sourceIndex];
			bool foundNext = false;
			void* next = static_cast<void*>(static_cast<char*>(source.myAddress) + source.mySize);
			for (targetIndex = 0; targetIndex < unpacked.size(); targetIndex++)
			{
				target = unpacked[targetIndex];

				if (target.myAddress == next)
				{
					foundNext = true;
					break;
				}
			}

			if (foundNext)
			{
				anythingLeft = true;
				Memory::Segment merged = { source.myAddress,source.mySize + target.mySize };
				unpacked.erase(unpacked.begin() + ((targetIndex > sourceIndex) ? targetIndex : sourceIndex));
				unpacked.erase(unpacked.begin() + ((targetIndex > sourceIndex) ? sourceIndex : targetIndex));
				unpacked.push_back(merged);

				break;
			}
		}
	}
	for (auto& i : unpacked)
	{
		mySegments[i.mySize].push(i); //repack
	}

}

NewFromMemoryBlock::NewFromMemoryBlock(MemoryChunk* aChunk) : myChunk(aChunk)
{
}

void* NewFromMemoryBlock::New(size_t size)
{
	return myChunk->Get(size);
}

size_t NewFromMemoryBlock::GetAvailability()
{
	return myChunk->GetAvailability();
}

size_t NewFromMemoryBlock::GetFragmentation()
{
	return myChunk->GetFragmentation();
}

void NewFromMemoryBlock::Delete(void* aPtr)
{
	myChunk->Return(aPtr);
}