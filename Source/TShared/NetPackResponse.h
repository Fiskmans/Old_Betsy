#pragma once
#include "NetMessage.h"

class NetPackResponse : public NetMessage
{
public:
	NetPackResponse();
	~NetPackResponse() = default;

	NetMessageIdType myPackageID;
	size_t myHash;	
};

