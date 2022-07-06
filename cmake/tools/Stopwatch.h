#ifndef TOOLS_STOPWATCH_H
#define TOOLS_STOPWATCH_H

#include "tools/TimeHelper.h"
#include "logger/Logger.h"

namespace tools
{
	class Stopwatch
	{
	public: 
		Stopwatch() = default;

		inline void Start()
		{
			myStartTime = GetTotalTime();
		}

		inline void Stop()
		{
			myTime = GetTotalTime() - myStartTime;
		}

		inline float Read()
		{
			return myTime;
		}

	private:

		float myTime = 0.f;
		float myStartTime = 0.f;
	};

	class TimedScope
	{
	public:
		inline TimedScope(Stopwatch& aStopWatch)
			: myStopWatch(aStopWatch)
		{
			myStopWatch.Start();
		}

		inline ~TimedScope()
		{
			myStopWatch.Stop();
			LOG_SYS_VERBOSE("timed scope finished in " + std::to_string(myStopWatch.Read()) + " seconds");
		}

	private:
		Stopwatch& myStopWatch;
	};
}

#endif