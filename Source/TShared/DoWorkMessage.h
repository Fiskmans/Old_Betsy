#pragma once
#include "NetMessage.h"
class DoWorkMessage :
	public NetMessage
{
public:
	DoWorkMessage();

	float aAmountOfWork;
	char myUserData;
};

