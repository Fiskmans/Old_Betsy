#pragma once
#include <CommonNetworkIncludes.h>
#include <functional>
#include <NetworkInterface.h>
#include "DeathMarker.h"

class MoveMessage;
class StatusMessage;
class BigFile;
class GameServer;


typedef  std::function<void(const NetMessage&)> SendFunction;

class Connection : public NetworkInterface
{
public:
	Connection();
	~Connection();
	Connection(sockaddr_in aAddress, int aAddressSize, SOCKET aSocket, unsigned short aID, GameServer* aGameServer);

	bool IsAlive();

	template<class T>
	void Send(const T& aData);
	void Send(const char* aData, int aDataSize, sockaddr* aCustomAddress = nullptr) override;
	void Receive(char* someData, const int aDataSize) override;
	void Invalidate();

	void Flush();

	unsigned short GetID();
	std::string GetName();

	sockaddr GetAddress();

	void AnnounceDeath(DeathMarker aMarker);
private:

	bool Evaluate(MoveMessage* aMessage);

	bool HandShake(char* aData, int aAmount);
	void Parse(char* aData, int aAmount);


	float myLastAction;
	bool myIsValid;
	char myConnectedUser[MAXUSERNAMELENGTH];
	unsigned short myID;

	SOCKET mySocket;

	sockaddr_in myAddress;
	int myAddressSize;
	GameServer* myServer;

	// Inherited via NetworkInterface
	virtual void TimedOut() override;
};

template<class T>
inline void Connection::Send(const T& aData)
{
	PreProcessAndSend(&aData,sizeof(aData));
}