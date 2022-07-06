#ifndef TOOLS_EVENT_H
#define TOOLS_EVENT_H

#include <functional>
#include <unordered_map>

#include "logger/Logger.h"

namespace tools
{
	using EventID = size_t;
	const EventID NullEventId = 0;

	inline EventID NextID()
	{
		static EventID CurrentID = NullEventId;
		return ++CurrentID;
	}

	template<typename... Args>
	class Event
	{
	public:
		EventID	Register(std::function<void(Args...)> aCallback);
		void UnRegister(EventID aEventId);

		void Fire(Args... aArgs) const;

	private:
		std::unordered_map<EventID, std::function<void(Args...)>> myCallbacks;
	};

	template<typename ...Args>
	inline EventID Event<Args...>::Register(std::function<void(Args...)> aCallback)
	{
		EventID id = NextID();
		myCallbacks.emplace(id, aCallback);
		return id;
	}

	template<typename ...Args>
	inline void Event<Args...>::UnRegister(EventID aEventId)
	{
		if (aEventId == NullEventId)
		{
			LOG_ERROR("Unregistering null eventid");
			return;
		}

		typename decltype(myCallbacks)::iterator it = myCallbacks.find(aEventId);
		if (it == myCallbacks.end())
		{
			LOG_ERROR("Callback not registered to this event");
			return;
		}

		myCallbacks.erase(it);
	}

	template<typename ...Args>
	inline void Event<Args...>::Fire(Args ...aArgs) const
	{
		for (const std::pair<EventID, std::function<void(Args...)>>& it : myCallbacks)
		{
			it.second(aArgs...);
		}
	}

}

#endif