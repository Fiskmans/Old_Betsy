#pragma once
#include "Message.hpp"

class Observer
{
public:
	Observer(const std::vector<MessageType>& aTypes = {});

	virtual ~Observer();
	virtual void RecieveMessage(const Message& aMessage) = 0;

private:
	const std::vector<MessageType> myTypes;
};

