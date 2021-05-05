#include <pch.h>
#include "Observer.hpp"
#include "PostMaster.hpp"

Observer::Observer()
{
}


Observer::~Observer()
{
}

void Observer::SubscribeToMessage(MessageType aMessageType)
{
	PostMaster::GetInstance()->Subscribe(aMessageType, this);
}

void Observer::UnSubscribeToMessage(MessageType aMessageType)
{
	PostMaster::GetInstance()->UnSubscribe(aMessageType, this);
}