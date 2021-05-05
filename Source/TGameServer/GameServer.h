#pragma once
#include <CommonNetworkIncludes.h>
#include <unordered_map>
#include "Connection.h"
#include <NetworkInterface.h>
#include "DeathMarker.h"

class GameServer
{
public:
	GameServer();
	~GameServer();
	void StartServer();

	void TransmitMessage(const NetMessage& aMessage);
	void LogDeath(Connection* aConnector, DeathMarker aMarker);
	std::vector<DeathMarker> GetDeaths();

private:
	void LoadDeaths();

	void Listen();

	void Flush();

	unsigned short myConnectionIdCounter = 3;
	float myLastRapportTime;
	size_t myTickCount;
	size_t myTargetTickRate = 20;

	unsigned short myUpstreamID;

	bool myIsHandshaking = false;
	bool myIsConnected = false;
	char myUpdateDistance = 0;

	SOCKET mySocket;

	std::vector<DeathMarker> myDeaths;
	std::vector<Connection*> myIncomingConnections;
	std::unordered_map<std::string, Connection> myClients;
};
