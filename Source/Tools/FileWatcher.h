#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <thread>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <atomic>

namespace Tools
{
	using CallbackFunction = void(const std::string&);

	class FileHandle : public std::string
	{
	public:
		size_t GetGUID() const { return myGUID; }
		bool GetHasStarted() const { return myFileLastChanged != 0; }
		FileHandle() : myFileLastChanged(0), myFileName(""), myHash(0), myGUID(0) {}
		FileHandle(const std::string& aString);
		bool CheckChanged() const; // NOT Const, hacky solution to allow using filehandle as key in std::map/std::unordered map and still be able to use it as normal
		bool operator==(const FileHandle& aOther) { return myHash == aOther.myHash && myGUID == aOther.myGUID && static_cast<std::string>(*this) == static_cast<std::string>(aOther); }

		private:
		time_t myFileLastChanged;
		std::string myFileName;
		size_t myHash;
		size_t myGUID;
		static size_t GUIDCounter;
	};
}

//Hashing functions

namespace std
{
	template<>
	struct hash<Tools::FileHandle>
	{
		std::size_t operator()(const Tools::FileHandle& k) const
		{
			return hash<std::string>()(k) + ~std::hash<size_t>()(k.GetGUID());
		}
	};
}

namespace Tools
{
	class FileWatcher
	{
	public:
		class UniqueID : public std::string
		{
		public:
			UniqueID() = default;
			~UniqueID() = default;
			UniqueID(const UniqueID&) = default;
			operator bool() { return !(*this == std::string()); } // returns whether the id is a valid id
		private:
			UniqueID(const std::string& aString) : std::string(aString) {}
			friend FileWatcher;
		};



		///Constructors
		FileWatcher();
		~FileWatcher();

		///Registers a filewatch with a callback, if using the callimmediately flag the function being called need to be thread safe
		UniqueID RegisterCallback(std::string aFile,std::function<CallbackFunction> aCallback, bool aCallImmediately = false);
		///Registers a file to manually watch, updates needs to be resolved using GetChangedFile
		UniqueID RegisterFile(std::string aFile);

		///Gets a changed file if there is one, returns whether there was one
		bool GetChangedFile(std::string& aOutFile);

		///Flushes all changed files
		void FlushChanges();

		///Unregisters a FileWatch
		bool UnRegister(UniqueID& aID);

		///Resets the filewatcher
		void UnRegisterAll();
	private:
		//Looping ThreadFunction
		void CheckFiles();

		//Locking Functions
		void StartLock();
		void EndLock();

		//the time to sleep between checks (in milliseconds)
		const int RefreshRate = 16; 


		//Flags
		std::atomic<bool> myIsRunning = true;
		std::atomic<bool> myHasPendingAdd = false;
		bool myIsPaused = false;

	
		//Registered files
		std::unordered_map <FileHandle, std::function<CallbackFunction>> myCallbackMap;
		std::unordered_map <FileHandle, std::function<CallbackFunction>> myThreadSafeMap;
		std::vector<FileHandle> mySelfHandleMap;

		//This filewatcher is using a lossy check with a single handover-point for simplicity,
		//meaning a single resolvepass (FushChanges/GetChangedFile) will process at most one file at a time.
		//To be clear the watcher won't miss any files, it'll just take a few passes to resolve everything, if a lot happens at once.
		//if this becomes a bottleneck it's recomended to switch these out for a couple of threadsafe queues
		std::string* myHandOver = nullptr;
		std::function<void()>* myFunctionHandOver = nullptr;

		std::thread myWorkHorse; //needs to be last because it uses the above resources
	};

}