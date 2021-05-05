#include <pch.h>
#include "ThreadPool.hpp"
#include<iostream>
#include <queue>
ThreadPool* ThreadPool::ourInstance = nullptr;

void ThreadPool::Create()
{
	assert(ourInstance == nullptr && "ThreadPool already created");
	ourInstance = new ThreadPool();

}

void ThreadPool::Destroy()
{
	assert(ourInstance != nullptr && "ThreadPool does not exist");
	delete ourInstance;
}

ThreadPool * ThreadPool::GetInstance()
{
	assert(ourInstance != nullptr && "Trying to getinstance from none existing threadpool");
	return ourInstance;
}

void ThreadPool::GiveWork(Work aWork)
{
	std::unique_lock<std::mutex> giveWorkLock(myLock);

	myWork.push(aWork);

	myConditional.notify_one();
}

void ThreadPool::DoWork()
{
	while (!myIsExiting)
	{
		Work work = GetNextWork();
		work.myWork();
	}
}

Work ThreadPool::GetNextWork()
{
	std::unique_lock<std::mutex> waitForWorkLock(myLock);
	myConditional.wait(waitForWorkLock, [this]()
	{
		return !(this->myWork.empty());
	});
	Work returnValue = myWork.front();
	myWork.pop();
	return returnValue;
}

void ThreadPool::StopAllThreadsForExit()
{
	std::unique_lock<std::mutex> myExitSyncLock(myLock);

	for (std::thread& thread : myThreads)
	{
		myWork.push(Work([this]() 
		{
			std::unique_lock<std::mutex> myExitSyncLock(this->myLock);
			this->myIsExiting = true;
		}));
	}
}

ThreadPool::ThreadPool()
{
	std::condition_variable threadCounterBarrier;
	int threadCount = std::thread::hardware_concurrency();
	int currentThreadsCreated = 0;
	for (int threadCreationIndex = 0; threadCreationIndex < threadCount ; ++threadCreationIndex)
	{
		myWork.push(Work([this, &currentThreadsCreated, &threadCounterBarrier]()
		{
			std::unique_lock<std::mutex> initSyncLock(this->myLock);
			++currentThreadsCreated;

			threadCounterBarrier.notify_one();
			
			this->myStartUpFinishedBarrier.wait(initSyncLock);
		}
		));
		myThreads.emplace_back(&ThreadPool::DoWork, this);
	}

	std::unique_lock<std::mutex> threadCreationWaitLock(myLock);
	threadCounterBarrier.wait(threadCreationWaitLock, [this, &currentThreadsCreated]()
	{
		return currentThreadsCreated == this->myThreads.size();
	});

	myStartUpFinishedBarrier.notify_all();
}


ThreadPool::~ThreadPool()
{

	StopAllThreadsForExit();


	myConditional.notify_all();

	for (std::thread& thread : myThreads)
	{
		thread.join();
	}
}
