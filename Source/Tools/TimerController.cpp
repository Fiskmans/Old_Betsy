#include <pch.h>
#include "TimerController.h"
#include "TimeHelper.h"

size_t TimerController::AddCallback(std::function<void()> aFunction, float aTime)
{
	return AddCallback(aFunction,aTime,-1.f);
}

size_t TimerController::AddCallback(std::function<void()> aFunction, float aTime, float aRepeat)
{
	Callback res;
	res.myFunction = aFunction;
	res.myTime = aTime + Tools::GetTotalTime();
	res.myRepeatInterval = aRepeat;
	res.myId = myIdCounter++;
	myTimers.push_back(res);
	return res.myId;
}

void TimerController::CheckTimers()
{
	float now = Tools::GetTotalTime();
	for (int i = int(myTimers.size())-1; i >= 0; i--)
	{
		auto& timer = myTimers[i];
		if (timer.myTime < now)
		{
			timer.myFunction();
			if (timer.myRepeatInterval > 0.f)
			{
				timer.myTime += timer.myRepeatInterval;
			}
			else
			{
				myTimers.erase(myTimers.begin() + i);
			}
		}
	}
}

bool TimerController::RemoveTimer(size_t aId) noexcept
{
	for (size_t i = 0; i < myTimers.size(); i++)
	{
		if (myTimers[i].myId == aId)
		{
			myTimers.erase(myTimers.begin() + i);
			return true;
		}
	}
	return false;
}
