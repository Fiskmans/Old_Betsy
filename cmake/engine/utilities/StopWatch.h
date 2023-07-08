#ifndef ENGINE_UTILITIES_STOPWATCH_H
#define ENGINE_UTILITIES_STOPWATCH_H

#include "engine/Time.h"

namespace engine::utilities
{
	class StopWatch
	{
	public:
		StopWatch()
		{
			myStart = Time::GetInstance().Now();
			myEnd = time::Seconds(0);
		}

		time::Seconds Result()
		{
			return myEnd - myStart;
		}

		time::Seconds Stop()
		{
			myEnd = Time::GetInstance().Now();
			return Result();
		}

	private:
		time::Seconds myStart;
		time::Seconds myEnd;
	};
}

#endif