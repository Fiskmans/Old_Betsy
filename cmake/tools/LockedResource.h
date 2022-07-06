#ifndef TOOLS_LOCKED_RESOURCE_H
#define TOOLS_LOCKED_RESOURCE_H

#include <mutex>

namespace tools
{

	template<class Type>
	class LockedResource
	{
	public:
		LockedResource(const LockedResource&) = delete;
		LockedResource(LockedResource&& aOther)
			: myMutex(aOther.myMutex)
			, myResource(aOther.myResource)
		{
			aOther.myMutex = nullptr;
		}

		LockedResource(std::mutex& aMutex, Type& aResource)
			: myMutex(&aMutex)
			, myResource(aResource)
		{
			myMutex->lock();
		}
		~LockedResource()
		{
			if (myMutex)
				myMutex->unlock();
		}



		Type& Get() { return myResource; }

	private:
		std::mutex* myMutex;
		Type& myResource;
	};
}
#endif