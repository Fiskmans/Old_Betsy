#include <pch.h>
#include "Publisher.hpp"

void Publisher::SendMessages(const Message& aMessage)
{
	PostMaster::GetInstance()->SendMessages(aMessage);
}

void Publisher::SendMessages(MessageType aMessageType)
{
	Message aMessage;
	aMessage.myMessageType = aMessageType;
	PostMaster::GetInstance()->SendMessages(aMessage);
}
