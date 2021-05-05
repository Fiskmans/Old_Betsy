#pragma once
#include "NetMessage.h"
#include "EasyAssignString.h"

#ifdef __INTELLISENSE__
#include <pch.h>
#endif 


namespace IdentificationTypes
{
	struct IsServer
	{
		EasyAssignString<128> myName;
		unsigned short myPort;
	};

	struct IsClient
	{
		EasyAssignString<128> myUsername;
	};

	struct IsHost
	{

	};
}
class NetIdentify : public NetMessage
{
public:
	enum class IdentificationType : char
	{
		Invalid,
		IsServer,
		IsClient,
		IsHost
	};
	NetIdentify();

	IdentificationType myProcessType;
	union
	{
		IdentificationTypes::IsServer myIsServer;
		IdentificationTypes::IsClient myIsClient;
		IdentificationTypes::IsHost myIsHost;
	};

};

