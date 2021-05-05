#pragma once
#include <functional>
#include <vector>

class TimerController
{
	struct Callback
	{
		float myTime;
		std::function<void()> myFunction;
		float myRepeatInterval;
		size_t myId;
	};


public:
	size_t AddCallback(std::function<void()> aFunction, float aTime);
	_NODISCARD size_t AddCallback(std::function<void()> aFunction, float aTime,float aRepeat);

	void CheckTimers();
	bool RemoveTimer(size_t aId) noexcept;
private:
	size_t myIdCounter;
	std::vector<Callback> myTimers;
};

