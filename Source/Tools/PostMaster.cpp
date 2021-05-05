#include <pch.h>
#include <iostream>
#include "PostMaster.hpp"
#include "Observer.hpp"
#include "assert.h"
PostMaster* PostMaster::ourInstance = nullptr;

PostMaster::PostMaster()
{
}


PostMaster::~PostMaster()
{
	for (auto mapIterator = mySubscriptions.begin(); mapIterator != mySubscriptions.end(); ++mapIterator)
	{
		std::vector<Observer*> observervector = (*mapIterator).second;
		observervector.clear();
	}
}

void PostMaster::Create()
{
	assert(ourInstance == nullptr && "Postmaster already Instantiated");
	ourInstance = new PostMaster();
}

void PostMaster::Destroy()
{
	assert(ourInstance != nullptr && "PostMaster not Instatiated");
	delete ourInstance;
	ourInstance = nullptr;
}

PostMaster* PostMaster::GetInstance()
{
	assert(ourInstance != nullptr && "PostMaster not Instatiated");

	return ourInstance;
}

void PostMaster::Subscribe(MessageType aMessageToSubscribe, Observer* aObserver)
{
	mySubscriptions[aMessageToSubscribe].push_back(aObserver);
}

void PostMaster::UnSubscribe(MessageType aMessageToSubscribe, Observer * aObserverToRemove)
{
	std::vector<Observer*>& aVectorToRemoveFrom = mySubscriptions[aMessageToSubscribe];
	for (int aObserverIndex = 0; aObserverIndex < aVectorToRemoveFrom.size(); ++aObserverIndex)
	{
		if (aVectorToRemoveFrom[aObserverIndex] == aObserverToRemove)
		{
			aVectorToRemoveFrom.erase(aVectorToRemoveFrom.begin() + aObserverIndex);
			break;
		}
	}
}

void PostMaster::SendMessages(const Message& aMessage)
{
	PERFORMANCETAG("sentmessagefrompostmaster");
	for (int subIndex = 0; subIndex < mySubscriptions[aMessage.myMessageType].size(); ++subIndex)
	{
		mySubscriptions[aMessage.myMessageType][subIndex]->RecieveMessage(aMessage);
	}
}

void PostMaster::SendMessages(MessageType aMessageType)
{
	Message msg;
	msg.myMessageType = aMessageType;
	SendMessages(msg);
}