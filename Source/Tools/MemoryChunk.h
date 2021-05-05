#pragma once


#include <vector>
#include <map>
#pragma push(_ENFORCE_MATCHING_ALLOCATORS)
#define _ENFORCE_MATCHING_ALLOCATORS 0
#include <unordered_map>
#include <stack>
#pragma pop
#include "Logger.h"

#include <memory>



class Allocator
{
public:
	virtual void* New(size_t size) = 0;

	virtual size_t GetAvailability() = 0;
	virtual size_t GetFragmentation() = 0;

	virtual void Delete(void* aPtr) = 0;
};


namespace Memory
{
	void SetGlobalAllocator(Allocator* aAllocator);
	void DefaultGlobalAllocator();
}


class NewFromOS : public Allocator
{
	// Inherited via Allocator
	virtual void* New(size_t size) override;
	virtual size_t GetAvailability() override;
	virtual size_t GetFragmentation() override;
	virtual void Delete(void* aPtr) override;
};



template <typename T>
class RawMallocAllocator
{
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	RawMallocAllocator() {}
	~RawMallocAllocator() {}

	template <class U> struct rebind { typedef RawMallocAllocator<U> other; };
	template <class U> RawMallocAllocator(const RawMallocAllocator<U>&) {}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }
	size_type max_size() const throw() { return size_t(-1) / sizeof(value_type); }

	pointer allocate(size_type n, const void* hint = 0)
	{
		return static_cast<pointer>(malloc(n * sizeof(T)));
	}

	void deallocate(pointer p, size_type n)
	{
		free(p);
	}

	void construct(pointer p, const T& val)
	{
		new(static_cast<void*>(p)) T(val);
	}

	void construct(pointer p)
	{
		new(static_cast<void*>(p)) T();
	}

	void destroy(pointer p)
	{
		p->~T();
	}
};

namespace Memory
{
	struct Segment
	{
		void* myAddress;
		size_t mySize;

		bool operator<(const Segment& aRhs) { return myAddress < aRhs.myAddress; }
		bool operator>(const Segment& aRhs) { return myAddress > aRhs.myAddress; }
		bool operator==(const Segment& aRhs) { return myAddress == aRhs.myAddress; }
	};
}

namespace std
{
	template<>
	struct hash<Memory::Segment>
	{
		size_t operator()(const Memory::Segment& x) const
		{
			return hash<void*>()(x.myAddress);
		}
	};
}

class MemoryChunk
{
public:

	MemoryChunk(size_t aSize);
	~MemoryChunk();

	_NODISCARD void* Get(size_t size);
	
	size_t GetAvailability();
	size_t GetFragmentation();

	void Return(void* aPtr);

	void DeFragment();

private:

	std::map<
		size_t, 
		std::stack<
			Memory::Segment,
			std::deque<
				Memory::Segment,
				RawMallocAllocator<
					Memory::Segment
				>
			>
		>, 
		std::less<size_t>,
		RawMallocAllocator<
			std::pair<
				const size_t, 
				std::stack<
					Memory::Segment,
					std::deque<
						Memory::Segment,
						RawMallocAllocator<
							Memory::Segment
						>
					>
				>
			>
		>
	> mySegments;

	//TOD: Get this to not scream
	std::unordered_map<void*, size_t,std::hash<void*>,std::equal_to<void*>/*,RawWindowsAllocator<std::pair<const void*,size_t>>*/> myInUse;
	void* myData;
	size_t myAvailable;
	const size_t Size;
};


class NewFromMemoryBlock : public Allocator
{
public:
	NewFromMemoryBlock(MemoryChunk* aChunk);

	// Inherited via Allocator
	virtual void* New(size_t size) override;
	virtual size_t GetAvailability() override;
	virtual size_t GetFragmentation() override;
	virtual void Delete(void* aPtr) override;

private:
	MemoryChunk* myChunk;
};


