#include <pch.h>
#include "FileWatcher.h"

#ifdef WIN32
#define stat _stat
#endif



namespace Tools
{
	size_t FileHandle::GUIDCounter = 0;

	FileWatcher::FileWatcher()
	{
		myWorkHorse = std::thread(std::bind(&FileWatcher::CheckFiles, this));
	}


	FileWatcher::~FileWatcher()
	{
		myIsRunning = false;
		if (myWorkHorse.joinable())
		{
			myWorkHorse.join();
		}
		int a = 10;
		//myWorkHorse.detach();
	}

	FileWatcher::UniqueID FileWatcher::RegisterCallback(std::string aFile, Tools::CallbackFunction aCallback, bool aCallImmediately)
	{
		static std::mutex mutex;
		std::lock_guard guard(mutex);
		StartLock();
		if (aCallImmediately)
		{
			myThreadSafeMap[aFile] = aCallback;
		}
		else
		{
			myCallbackMap[aFile] = aCallback;
		}
		EndLock();

		return UniqueID(aFile);
	}

	FileWatcher::UniqueID FileWatcher::RegisterFile(std::string aFile)
	{
		StartLock();
		mySelfHandleMap.push_back(aFile);
		EndLock();

		return UniqueID(aFile);
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

	bool FileWatcher::UnRegister(UniqueID& aID)
	{
		StartLock();
		if (myCallbackMap.count(aID) != 0)
		{
			myCallbackMap.erase(aID);
			EndLock();
			aID = UniqueID();
			return true;
		}
		if (myThreadSafeMap.count(aID) != 0)
		{
			myThreadSafeMap.erase(aID);
			EndLock();
			aID = UniqueID();
			return true;
		}
		for (size_t i = 0; i < mySelfHandleMap.size(); i++)
		{
			if (std::string(mySelfHandleMap[i]) == aID)
			{
				mySelfHandleMap.erase(mySelfHandleMap.begin() + i);
				EndLock();
				aID = UniqueID();
				return true;
			}
		}
		EndLock();
		return false;
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
				if (!i.first.GetHasStarted())
				{
					i.first.CheckChanged();
				}
				if (i.first.CheckChanged())
				{
					i.second(i.first);
				}
			}
			for (auto& i : myCallbackMap)
			{
				if (!i.first.GetHasStarted())
				{
					i.first.CheckChanged();
				}
				if (!myFunctionHandOver && i.first.CheckChanged())
				{
					holder = std::bind(i.second, i.first);
					myFunctionHandOver = &holder;
				}
			}
			for (auto& i : mySelfHandleMap)
			{
				if (!i.GetHasStarted())
				{
					i.CheckChanged();
				}
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

	FileHandle::FileHandle(const std::string& aString) : myFileName(std::experimental::filesystem::canonical(aString).generic_string()), std::string(aString), myFileLastChanged(0)
	{ 
		myGUID = GUIDCounter++;
		myHash = std::hash<FileHandle>()(*this); 
	}
	
	

	bool FileHandle::CheckChanged() const
	{
		time_t lastChanged = Tools::FileLastModified(myFileName);

		if (lastChanged != -1 && lastChanged > myFileLastChanged)
		{
			*const_cast<time_t*>(&myFileLastChanged) = lastChanged;	//const to allow whole object to be used as a key in a std::map, 
																		//changing time does not affect the hash of it so this is 'safe' (TM)
			return true;
		}
		return false;
	}

}