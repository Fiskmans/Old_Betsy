#pragma once
#include "NetMessage.h"
#include "EasyAssignString.h"

namespace NetRapportTypes
{
	enum class Types : char
	{
		Invalid,
		Name,
		Connection,
		Configuration,
		Load
	};

	enum class ConfigurationType : char
	{
		Invalid,
		Lobby,
		Upstream,
		Host,
		Client
	};

	struct Name
	{
		EasyAssignString<128> myName;
	};

	struct Connection
	{
		sockaddr myConnection;
		bool myIsSameProgram;
	};
	struct Configuration
	{
		ConfigurationType myConfiguration;
	};
	struct Load
	{
		float aLoadAmount;
	};
}


class NetRapport :
	public NetMessage
{
public:
	NetRapport();

	NetRapportTypes::Types myRapportType;
	union
	{
		NetRapportTypes::Name myNameType;
		NetRapportTypes::Connection myConnectionType;
		NetRapportTypes::Configuration myConfigurationType;
		NetRapportTypes::Load myLoadType;
	};

};

