#include <pch.h>
#include "Observer.hpp"
#include "PostMaster.hpp"


Observer::Observer(const std::vector<MessageType>& aTypes)
	: myTypes(aTypes)
{
	PostMaster& pm = PostMaster::GetInstance();
	for (const MessageType& type : myTypes)
	{
		pm.Subscribe(this, type);
	}
}

Observer::~Observer()
{
	PostMaster& pm = PostMaster::GetInstance();
	for (const MessageType& type : myTypes)
	{
		if (!pm.UnSubscribe(this, type))
		{
			SYSERROR("Observer not subscribed to event it should be");
		}
	}
}