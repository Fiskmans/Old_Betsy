#pragma once
#include <vector>
#include <unordered_map>
#include "Message.hpp"

class Observer;

class PostMaster
{
public:

	static void Create();
	static void Destroy();
	static PostMaster* GetInstance();
	void Subscribe(MessageType aMessageToSubscribe, Observer* aObserver);
	void UnSubscribe(MessageType aMessageToSubscribe, Observer* aObserverToRemove);
	void SendMessages(const Message& aMessage);
	void SendMessages(MessageType aMessageType);


private:
	static PostMaster* ourInstance;
	PostMaster();
	~PostMaster();
	std::unordered_map<MessageType, std::vector<Observer*>> mySubscriptions;
};