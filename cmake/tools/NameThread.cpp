
#include "tools/NameThread.h"
#include "tools/Singleton.h"
#include "tools/LockedResource.h"

#include <unordered_map>
#include <mutex>

#define NOMINMAX
#include <windows.h>

namespace tools
{

	class ThreadNameManager : public tools::Singleton<ThreadNameManager>
	{
	public:

		void Register(std::string aName)
		{
			std::lock_guard lock(myMutex);
			myMap.emplace(std::this_thread::get_id(), aName);
		}

		void UnRegister()
		{
			std::lock_guard lock(myMutex);
			myMap.erase(myMap.find(std::this_thread::get_id()));
		}

		LockedResource<std::unordered_map<std::thread::id, std::string>> Access() { return LockedResource<std::unordered_map<std::thread::id, std::string>>(myMutex, myMap); }

	private:
		std::unordered_map<std::thread::id, std::string> myMap;
		std::mutex myMutex;
	};

	class NameRegister
	{
	public:
		NameRegister(std::string aName)
		{
			ThreadNameManager::GetInstance().Register(aName);
		}
		~NameRegister()
		{
			ThreadNameManager::GetInstance().UnRegister();
		}
	};

	std::string GetNameOfThread(std::thread::id aThreadId)
	{
		LockedResource<std::unordered_map<std::thread::id, std::string>> map = ThreadNameManager::GetInstance().Access();
		std::unordered_map<std::thread::id, std::string>::iterator it = map.Get().find(aThreadId);
		
		if (it != map.Get().end())
			return it->second;

		return "unamed_thread";
	}

	void NameThread(const std::string& aName)
	{
		thread_local NameRegister nameRegistration(aName);
		static_cast<void>(nameRegistration);

		std::wstring name(aName.begin(), aName.end());

		SetThreadDescription(GetCurrentThread(), name.c_str());
	}
}