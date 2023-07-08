#ifndef ENIGNE_TIME_MANAGER_H
#define ENIGNE_TIME_MANAGER_H

#include "tools/Singleton.h"

#include <chrono>

namespace engine
{
	namespace time
	{
		using Milliseconds = std::chrono::duration<double, std::milli>;
		using Seconds = std::chrono::duration<double, std::ratio<1, 1>>;
	}

	class GameEngine;
	class Time : public fisk::tools::Singleton<Time>
	{
	public:

		Time();

		inline time::Seconds Now()
		{
			return myNow;
		}
		inline time::Seconds DeltaTime()
		{
			return myDeltaTime;
		}

	private:
		friend GameEngine;
		
		void Update();

		using Clock = std::chrono::steady_clock;

		Clock::time_point myEpoch;

		time::Seconds myNow;
		time::Seconds myDeltaTime;
	};
}
#endif