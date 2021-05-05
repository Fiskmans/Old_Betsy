#include <pch.h>
#include "EventSystem.hpp"

namespace CommonUtilities
{


	EventSystem::EventSystem()
	{
		myMutedEvents.Init(10);
		myDispatchQueue.Init(10);
	}

	EventSystem::~EventSystem()
	{
	}

	void EventSystem::Register(EVENT_LISTENER_ID_TYPE aListenerID, const EVENT_MESSAGE_ID_TYPE& aEventID, void(*aFunction)(const EventMessage*))
	{
		EventData data;
		data.listenerID = aListenerID;
		data.block = false;
		data.function = aFunction;
		Register(aEventID, data);
	}

	void EventSystem::Unregister(EVENT_LISTENER_ID_TYPE aListenerID, const EVENT_MESSAGE_ID_TYPE& aEventID)
	{
		auto range = myConnections.equal_range(aEventID);
		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->second.listenerID == aListenerID)
			{
				myConnections.erase(it);
				break;
			}
		}
	}

	void EventSystem::UnregisterAll(const EVENT_MESSAGE_ID_TYPE& aEventID)
	{
		myConnections.erase(aEventID);
	}

	void EventSystem::UnregisterAll(EVENT_LISTENER_ID_TYPE aListenerID)
	{

		for (std::multimap<EVENT_MESSAGE_ID_TYPE, EventData>::iterator it = myConnections.begin(); it != myConnections.end();)
		{
			if (it->second.listenerID == aListenerID)
			{
				it = myConnections.erase(it);
			}
			else
			{
				++it;
			}
		}

	}

	void EventSystem::Mute(const EVENT_MESSAGE_ID_TYPE& aEventID)
	{
		for (int i = 0; i < myMutedEvents.Size(); ++i)
		{
			if (myMutedEvents[i] == aEventID)
			{
				return;
			}
		}

		myMutedEvents.Add(aEventID);
	}

	void EventSystem::Block(EVENT_LISTENER_ID_TYPE aListenerID)
	{
		for (auto it = myConnections.begin(); it != myConnections.end(); ++it)
		{
			if (it->second.listenerID == aListenerID)
			{
				it->second.block = true;
			}
		}
	}

	void EventSystem::UnMute(const EVENT_MESSAGE_ID_TYPE& aEventID)
	{
		for (int i = 0; i < myMutedEvents.Size(); ++i)
		{
			if (myMutedEvents[i] == aEventID)
			{
				myMutedEvents.RemoveCyclicAtIndex(i);
				break;
			}
		}
	}

	void EventSystem::UnBlock(EVENT_LISTENER_ID_TYPE aListenerID)
	{
		for (auto it : myConnections)
		{
			if (it.second.listenerID == aListenerID)
			{
				it.second.block = false;
			}
		}
	}

	void EventSystem::Trigger(const EVENT_MESSAGE_ID_TYPE& aID, const EventMessage* aMessage)
	{
		myDispatchQueue.Add({ aID, aMessage });
	}

	void EventSystem::Trigger(const EVENT_MESSAGE_ID_TYPE& aID)
	{
		myDispatchQueue.Add({ aID, nullptr });
	}

	void EventSystem::TriggerAndDispatch(const EVENT_MESSAGE_ID_TYPE& aID)
	{
		Execute(aID, nullptr);
	}

	void EventSystem::TriggerAndDispatch(const EVENT_MESSAGE_ID_TYPE& aID, const EventMessage* aMessage)
	{
		Execute(aID, aMessage);
	}

	void EventSystem::Dispatch()
	{
		for (int index = 0; index < myDispatchQueue.Size(); ++index)
		{
			Execute(myDispatchQueue[index].id, myDispatchQueue[index].message);
		}

		//TODO: DeleteAll
		myDispatchQueue.RemoveAll();
	}

	void EventSystem::Register(const EVENT_MESSAGE_ID_TYPE& aEventID, EventData& aData)
	{
		auto range = myConnections.equal_range(aEventID);
		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->first == aEventID && it->second.listenerID == aData.listenerID)
			{
				return;
			}
		}

		myConnections.insert(std::make_pair(aEventID, aData));
	}

	void EventSystem::Execute(const EVENT_MESSAGE_ID_TYPE& aEventID, const EventMessage * aMessage)
	{
		for (int i = 0; i < myMutedEvents.Size(); ++i)
		{
			if (myMutedEvents[i] == aEventID)
			{
				return;
			}
		}

		auto range = myConnections.equal_range(aEventID);
		for (auto it = range.first; it != range.second; ++it)
		{
			if (!it->second.block)
			{
				it->second.function(aMessage);
			}
		}
	}
}