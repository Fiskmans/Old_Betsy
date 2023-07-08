
#include "engine/Time.h"

namespace engine
{
	Time::Time()
	{
		myEpoch = Clock::now();
	}

	void Time::Update()
	{
		time::Seconds now = Clock::now() - myEpoch;
		myDeltaTime = now - myNow;
		myNow = now;
	}
}

