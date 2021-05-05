#pragma once

#include <functional>
#include <map>

#include "Singleton.hpp"
#include "GrowingArray.hpp"

#include "EventSystemTypes.hpp"

#ifndef EVENT_MESSAGE_ID_TYPE
#define EVENT_MESSAGE_ID_TYPE int
#endif

#ifndef EVENT_LISTENER_ID_TYPE
#define EVENT_LISTENER_ID_TYPE int
#endif

namespace CommonUtilities
{
	struct EventMessage
	{
	};

	class EventSystem : public Singleton<EventSystem>
	{
		friend class Singleton<EventSystem>;
	public:
		EventSystem();
		~EventSystem();
		/*
		aListenerId: A unique identifier for the listener. Recommend using '(void*)this' for objects.
		aEVENT_MESSAGE_ID_TYPE: A unique identifier for the event.
		aFunction: Callback function.
		*/
		void Register(EVENT_LISTENER_ID_TYPE aListenerID, const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE, void(*aFunction)(const EventMessage*));
		/*
		aListenerId: A unique identifier for the listener. Recommend using '(void*)this' for objects.
		aEVENT_MESSAGE_ID_TYPE: A unique identifier for the event.
		aFunction: Callback function.
		aThis: Pointer to the object containing the callback function.
		*/
		template<typename T>
		void Register(EVENT_LISTENER_ID_TYPE aListenerID, const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE, void (T::*aFunction)(const EventMessage*), T* aThis);

		void Unregister(EVENT_LISTENER_ID_TYPE aListenerID, const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE);
		void UnregisterAll(const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE);
		void UnregisterAll(EVENT_LISTENER_ID_TYPE aListenerID);

		void Mute(const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE);
		void Block(EVENT_LISTENER_ID_TYPE aListenerID);
		void UnMute(const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE);
		void UnBlock(EVENT_LISTENER_ID_TYPE aListenerID);
		
		void Trigger(const EVENT_MESSAGE_ID_TYPE&, const EventMessage* aMessage);
		void Trigger(const EVENT_MESSAGE_ID_TYPE& aID);
		void TriggerAndDispatch(const EVENT_MESSAGE_ID_TYPE& aID);
		void TriggerAndDispatch(const EVENT_MESSAGE_ID_TYPE&, const EventMessage* aMessage);
		void Dispatch();
	private:

		struct EventData
		{
			std::function<void(const EventMessage*)> function;
			EVENT_LISTENER_ID_TYPE listenerID = nullptr;
			bool block = false;
		};

		struct DispatchData
		{
			EVENT_MESSAGE_ID_TYPE id;
			const EventMessage* message;
		};

		void Register(const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE, EventData& aData);
		void Execute(const EVENT_MESSAGE_ID_TYPE& aEventID, const EventMessage* aMessage);

		std::multimap<EVENT_MESSAGE_ID_TYPE, EventData> myConnections;
		GrowingArray<EVENT_MESSAGE_ID_TYPE> myMutedEvents;
		GrowingArray<DispatchData> myDispatchQueue;
	};
	
	template<typename T>
	inline void EventSystem::Register(EVENT_LISTENER_ID_TYPE aListenerID, const EVENT_MESSAGE_ID_TYPE& aEVENT_MESSAGE_ID_TYPE, void(T::* aFunction)(const EventMessage *), T * aThis)
	{
		EventData data;
		data.listenerID = aListenerID;
		data.block = false;
		data.function = std::bind(aFunction, aThis, std::placeholders::_1);
		Register(aEVENT_MESSAGE_ID_TYPE, data);
	}
}