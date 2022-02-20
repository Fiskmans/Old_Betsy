#pragma once
#include "Message.hpp"
class Secret
{
public:
	Secret(std::string aKeyCombination, MessageType aMessageType,bool aMultiUse = false);

	void Input(char aKey);
private:
	size_t myAt;
	bool myIsMultiUse;
	bool myHasActivated;
	std::string myKey;
	MessageType myMessageType;
};

