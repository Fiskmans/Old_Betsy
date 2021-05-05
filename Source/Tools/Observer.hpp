#pragma once
#include "Message.hpp"
//#include "CollideRectangle.hpp"


class Subject;

class Observer
{
public:
	virtual ~Observer();
	virtual void RecieveMessage(const Message& aMessage) = 0;
	void UnSubscribeToMessage(MessageType aMessageType);
	void SubscribeToMessage(MessageType aMessageType);

protected:
	Observer();
private:
	
};

