#pragma once
#include "Message.hpp"

#include "CommonUtilities\Singleton.hpp"

class Observer;

class PostMaster : public CommonUtilities::Singleton<PostMaster>
{
public:
	PostMaster();
	~PostMaster();

	void SendMessages(const Message& aMessage);
	void SendMessages(MessageType aMessageType, const void* aPayload = nullptr);


private:
	friend Observer;

	void Subscribe(Observer* aObserver,MessageType aMessageType);
	bool UnSubscribe(Observer* aObserver, MessageType aMessageType);

	std::unordered_map<MessageType, std::vector<Observer*>> mySubscriptions;
};