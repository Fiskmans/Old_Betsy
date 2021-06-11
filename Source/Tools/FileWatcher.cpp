#include <pch.h>
#include "FileWatcher.h"

#ifdef WIN32
#define stat _stat
#endif



namespace Tools
{
	FileWatcher::FileWatcher()
	{
		myWorkHorse = std::thread(std::bind(&FileWatcher::CheckFiles, this));
	}


	FileWatcher::~FileWatcher()
	{
		myIsRunning = false;
		myWorkHorse.join();
	}

	FileWatcherUniqueID FileWatcher::RegisterCallback(std::string aFile, CallbackFunction aCallback, bool aCallImmediately)
	{
		FileWatcherUniqueID id = aFile;
		static std::mutex mutex;
		std::lock_guard guard(mutex);
		StartLock();
		RegisteredFile registed
		{
			FileHandle(aFile),
			aCallback
		};

		if (aCallImmediately)
		{
			myThreadSafeMap[id] = registed;
		}
		else
		{
			myCallbackMap[id] = registed;
		}
		EndLock();

		return id;
	}

	FileWatcherUniqueID FileWatcher::RegisterFile(std::string aFile)
	{
		StartLock();
		mySelfHandleMap.push_back(aFile);
		EndLock();

		return FileWatcherUniqueID(aFile);
	}

	bool FileWatcher::GetChangedFile(std::string& aOutFile)
	{
		if (myHandOver)
		{
			aOutFile = *myHandOver;
			myHandOver = nullptr;
			return true;
		}
		return false;
	}

	void FileWatcher::FlushChanges()
	{
		if (myFunctionHandOver)
		{
			(*myFunctionHandOver)();
			myFunctionHandOver = nullptr;
		}
	}

	bool FileWatcher::UnRegister(FileWatcherUniqueID& aID)
	{
		StartLock();
		if (myCallbackMap.count(aID) != 0)
		{
			myCallbackMap.erase(aID);
			EndLock();
			aID = FileWatcherUniqueID();
			return true;
		}
		if (myThreadSafeMap.count(aID) != 0)
		{
			myThreadSafeMap.erase(aID);
			EndLock();
			aID = FileWatcherUniqueID();
			return true;
		}
		for (size_t i = 0; i < mySelfHandleMap.size(); i++)
		{
			if (mySelfHandleMap[i] == aID)
			{
				mySelfHandleMap.erase(mySelfHandleMap.begin() + i);
				EndLock();
				aID = FileWatcherUniqueID();
				return true;
			}
		}
		EndLock();
		return false;
	}

	void FileWatcher::Trigger(FileWatcherUniqueID& aID)
	{
		StartLock();
		if (myCallbackMap.count(aID) != 0)
		{
			myCallbackMap[aID].myFunction(aID);
		}
		if (myThreadSafeMap.count(aID) != 0)
		{
			myThreadSafeMap[aID].myFunction(aID);
		}
		for (auto& i : mySelfHandleMap)
		{
			if (i == aID)
			{
				if (&myHandOver && i.CheckChanged())
				{
					myHandOver = &i;
				}
			}
		}
		EndLock();
	}

	void FileWatcher::UnRegisterAll()
	{
		StartLock();
		myCallbackMap.clear();
		myThreadSafeMap.clear();
		mySelfHandleMap.clear();
		EndLock();
	}

	void FileWatcher::CheckFiles()
	{
		NAMETHREAD(L"FILEWATCHER-Workhorse")
		std::function<void()> holder;
		mySelfHandleMap = std::vector<FileHandle>();
		while (myIsRunning)
		{
		#pragma region Locking
			if (myHasPendingAdd)
			{
				myIsPaused = true;
				while (myIsPaused)
				{
					std::this_thread::yield();
				}
			}
		#pragma endregion

			for (auto& i : myThreadSafeMap)
			{
				if (i.second.myHandle.CheckChanged())
				{
					i.second.myFunction(i.first);
				}
			}
			for (auto& i : myCallbackMap)
			{
				if (!myFunctionHandOver && i.second.myHandle.CheckChanged())
				{
					holder = std::bind(i.second.myFunction, i.first);
					myFunctionHandOver = &holder;
				}
			}
			for (auto& i : mySelfHandleMap)
			{
				if (&myHandOver && i.CheckChanged())
				{
					myHandOver = &i;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(RefreshRate));
		}
	}

	void FileWatcher::StartLock()
	{
		myHasPendingAdd = true;
		while (!myIsPaused)
		{
			std::this_thread::yield();
		}
	}

	void FileWatcher::EndLock()
	{
		myHasPendingAdd = false;
		myIsPaused = false;
	}

	FileWatcher::FileHandle::FileHandle(const std::string& aString) : std::string(aString)
	{
	}

	bool FileWatcher::FileHandle::CheckChanged()
	{
		time_t lastChanged = Tools::FileLastModified(myFileName);

		if (lastChanged != -1 && lastChanged > myFileLastChanged)
		{
			myFileLastChanged = lastChanged;	//const to allow whole object to be used as a key in a std::map, 
																		//changing time does not affect the hash of it so this is 'safe' (TM)
			return true;
		}
		return false;
	}

}