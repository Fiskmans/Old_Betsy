#include <pch.h>
#include "Secret.h"
#include "PostMaster.hpp"
#include "Logger.h"

Secret::Secret(std::string aKeyCombination, MessageType aMessageType, bool aMultiUse)
{
	myKey = aKeyCombination;
	myHasActivated = false;
	myAt = 0;
	myMessageType = aMessageType;
	myIsMultiUse = aMultiUse;
}

void Secret::Input(char aKey)
{
	if (!myIsMultiUse && myHasActivated)
	{
		return;
	}
	if (myKey[myAt] == aKey)
	{
		myAt++;
		if (myAt == myKey.size())
		{
			myHasActivated = true;
			PostMaster::GetInstance().SendMessages(myMessageType);
		}
	}
	else
	{
		myAt = 0;
	}

}
