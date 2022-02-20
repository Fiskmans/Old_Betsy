#pragma once

namespace Tools
{
	class FileWatcherUniqueID : public std::string
	{
	public:
		FileWatcherUniqueID() = default;
		~FileWatcherUniqueID() = default;
		FileWatcherUniqueID(const FileWatcherUniqueID&) = default;
		operator bool() { return !(*this == std::string()); } // returns whether the id is a valid id
	private:
		FileWatcherUniqueID(const std::string& aString) : std::string(aString) {}
		friend class FileWatcher;
	};
}

namespace std
{
	template<>
	class hash<Tools::FileWatcherUniqueID>
	{
	public:
		size_t operator()(const Tools::FileWatcherUniqueID& id) const
		{
			return hash<string>()(id);
		}
	};
}

namespace Tools
{
	class FileWatcher
	{
	public:
		typedef std::function<void(const std::string&)> CallbackFunction;

		///Constructors
		FileWatcher();
		~FileWatcher();

		///Registers a filewatch with a callback, if using the callimmediately flag the function being called needs to be thread safe
		FileWatcherUniqueID RegisterCallback(std::string aFile, CallbackFunction aCallback, bool aCallImmediately = false);
		///Registers a file to manually watch, updates needs to be resolved using GetChangedFile
		FileWatcherUniqueID RegisterFile(std::string aFile);

		///Gets a changed file if there is one, returns whether there was one
		bool GetChangedFile(std::string& aOutFile);

		///Flushes all changed files
		void FlushChanges();

		///Unregisters a FileWatch
		bool UnRegister(FileWatcherUniqueID& aID);

		void Trigger(FileWatcherUniqueID& aID);

		///Resets the filewatcher
		void UnRegisterAll();
	private:

		class FileHandle : public std::string
		{
		public:
			FileHandle() : myFileLastChanged(0), myFileName("") { CheckChanged(); }
			FileHandle(const std::string& aString);
			bool CheckChanged();

		private:
			time_t myFileLastChanged;
			std::string myFileName;
		};

		struct RegisteredFile
		{
			FileHandle myHandle;
			CallbackFunction myFunction;
		};


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
		std::atomic<bool> myIsPaused = false;

	
		//Registered files
		std::unordered_map <FileWatcherUniqueID, RegisteredFile> myCallbackMap;
		std::unordered_map <FileWatcherUniqueID, RegisteredFile> myThreadSafeMap;
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