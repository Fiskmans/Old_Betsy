#pragma once
#include "NetMessage.h"
#include <string>
#include "EasyAssignString.h"

class StatusMessage : public NetMessage
{
public:
	StatusMessage();
	enum class Status : char
	{
		UserConnected,
		UserOnline,
		UserDisconnected,
		PotentialServer,
		EvaluatedServer,
		ConnectToServer,
		LoadRapport,
		WantRehost,
		RehostResponse,
		OffloadOppertunity
	} myStatus;

	sockaddr myAddress;
	union
	{
		EasyAssignString<128> myUsername;
		struct PotentialServer
		{
			char aAddressSize;
		} myServer;
		struct EvaluatedServer
		{
			float aPing;
		} myEvalServer;
		float myLoad;
		struct RehostResponse
		{
			sockaddr myRehostTo;
			bool myIsValidEndpoint;
		} myRehostResponse;
	};
	unsigned short myAssignedID;

};

