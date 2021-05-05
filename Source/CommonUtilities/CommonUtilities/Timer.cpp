#include <pch.h>
#include "Timer.hpp"

namespace CommonUtilities
{
	GenericTimer::GenericTimer() :
		myDuration(0.0f),
		myTime (0.0f)
	{
	}

	GenericTimer::GenericTimer(float aDuration = 0.0f)
	{
		Start(aDuration);
	}

	void GenericTimer::Update(float aDeltaTime)
	{
		if (myTime > 0.0f)
		{
			myTime -= aDeltaTime;
			if (myTime < 0)
			{
				myTime = 0;
			}
		}
	}

	void GenericTimer::Start(float aDuration)
	{
		if (aDuration < 0.0f)
		{
			myDuration = 0.0f;
			myTime = 0.0f;
		}
		else
		{
			myDuration = aDuration;
			myTime = aDuration;
		}
	}

	bool GenericTimer::IsDone() const
	{
		return myTime == 0.0f;
	}

	float GenericTimer::GetProgress() const
	{
		if (myDuration > 0.0f)
		{
			return myTime / myDuration;
		}
		else
		{
			return 1.0f;
		}
	}

	StopWatch::StopWatch() :
		myTimeScale(1.0f),
		myDeltaTime(std::chrono::duration<float>(0.0f))
	{
		Reset();

	}

	void StopWatch::Tick()
	{
		myDeltaTime = std::chrono::high_resolution_clock::now() - myStartTime;
	}

	void StopWatch::Reset()
	{
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	void StopWatch::SetTimeScale(float aScale)
	{
		if (aScale >= 0)
		{
			myTimeScale = aScale;
		}
	}

	float StopWatch::GetTimeScale() const
	{
		return myTimeScale;
	}

	float StopWatch::GetDeltaTime() const
	{
		return myDeltaTime.count();
	}

	FrameTimer::FrameTimer() :
		myDeltaTime(std::chrono::duration<float>(0.0f)),
		myTimeScale(1.0f)
	{
		myCreationTime = std::chrono::high_resolution_clock::now();
		myLastFrame = myCreationTime;
	}

	void FrameTimer::Update()
	{
		myCurrentFrame = std::chrono::high_resolution_clock::now();
		myDeltaTime = myCurrentFrame - myLastFrame;
		myLastFrame = myCurrentFrame;
	}

	float FrameTimer::GetDeltaTime() const
	{
		return myDeltaTime.count();
	}

	double FrameTimer::GetTotalTime() const
	{
		return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myCreationTime).count();
	}
}
