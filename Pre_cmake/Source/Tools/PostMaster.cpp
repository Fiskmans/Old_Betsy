#include <pch.h>
#include "PostMaster.hpp"

PostMaster::PostMaster()
{
}


PostMaster::~PostMaster()
{
	mySubscriptions.clear();
}

void PostMaster::SendMessages(const Message& aMessage)
{
	PERFORMANCETAG("sentmessagefrompostmaster");
	for (auto& observer : mySubscriptions[aMessage.myMessageType])
	{
		observer->RecieveMessage(aMessage);
	}
}

void PostMaster::SendMessages(MessageType aMessageType, const void* aPayload)
{
	Message msg;
	msg.myMessageType = aMessageType;
	msg.myData = aPayload;
	SendMessages(msg);
}

void PostMaster::Subscribe(Observer* aObserver, MessageType aMessageType)
{
	mySubscriptions[aMessageType].push_back(aObserver);
}

bool PostMaster::UnSubscribe(Observer* aObserver, MessageType aMessageType)
{
	auto& list = mySubscriptions[aMessageType];
	auto it = std::find(list.begin(), list.end(), aObserver);
	if (it == list.end())
	{
		return false;
	}
	list.erase(it);
	return true;
}
