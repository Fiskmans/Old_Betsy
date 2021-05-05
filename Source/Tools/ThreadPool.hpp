#pragma once
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include "Work.hpp"
#include "assert.h"
#include <condition_variable>
#include <atomic>
class ThreadPool
{
public:
	static void Create();
	static void Destroy();
	static ThreadPool* GetInstance();
	void GiveWork(Work aWork);
private:
	void DoWork();
	Work GetNextWork();
	void StopAllThreadsForExit();
	ThreadPool();
	~ThreadPool();
	static ThreadPool* ourInstance;
	std::vector<std::thread> myThreads;
	std::queue<Work> myWork;
	std::atomic_bool myIsExiting = false;
	std::condition_variable myConditional;
	std::condition_variable myStartUpFinishedBarrier;
	std::mutex myLock;
};

