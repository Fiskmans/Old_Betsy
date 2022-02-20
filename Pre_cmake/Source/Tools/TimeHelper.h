#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <thread>

namespace Tools
{
	class ScopeDiagnostic
	{
	public:
		ScopeDiagnostic(const char* aName);
		~ScopeDiagnostic();
	};

	struct TimeTree
	{
		float myTime;
		float myCovarage;
		float myTimeStamp;
		size_t myCallCount;
		const char* myName;
		TimeTree* myParent = nullptr;
		std::vector<TimeTree*> myChildren;
	};

	inline float GetTotalTime()
	{
		long long now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
		static long long CreationTime = now;
		return float((now - CreationTime) / double(1000.0 * 1000.0));
	}

	inline std::unordered_map<std::string, float>& GetOpenDiagnostics()
	{
		static std::unordered_map<std::string, float> out;
		return out;
	}

	inline void StartDiagnostic(const std::string& aKey) noexcept
	{
		GetOpenDiagnostics()[aKey] = GetTotalTime();
	}

	inline float EndDiagnostic(const std::string& aKey) noexcept
	{
		return  GetTotalTime() - GetOpenDiagnostics()[aKey];
	}

	std::unordered_map<std::thread::id, TimeTree*>& GetAllRoots();

	TimeTree* GetTimeTreeRoot();
	void PushTimeStamp(const char* aName);
	float PopTimeStamp();
	void FlushTimeTree();
	
}

