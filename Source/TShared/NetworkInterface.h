#pragma once
#include "NetMessage.h"
#include <vector>
#include <functional>
#include <unordered_map>

struct SentMessageTracker
{
	char package[MAXPACKAGESIZE];
	size_t expectedHash;
	float lastSent;
	NetMessageIdType ID;
	short packageSize;
	char sentCount;
	bool myHasCustomAddress;
	sockaddr myCustomAddress;
};



class NetworkInterface
{
public:
	NetworkInterface() = default;
	~NetworkInterface() = default;

	virtual void Receive(char* someData, const int aDataSize) = 0;
	virtual void Send(const char* someData, const int aDataSize, sockaddr* aCustomAddress = nullptr) = 0;
	virtual void TimedOut() = 0;

	void PreProcessAndSend(const NetMessage* aMessage, const int aDataSize, sockaddr* aCustomAddress = nullptr);
	bool PreProcessReceive(char* someData, const int aDataSize);
	
	void Flush();


	void Clear();

	void HookCallBack(NetMessageIdType aID, std::function<void(NetMessageIdType)> aFunction);
private:
	std::vector<SentMessageTracker> myPendingAccepts;
	std::vector<std::pair<NetMessageIdType, float>> myRecentMesseges;
	std::unordered_map<NetMessageIdType, std::function<void(NetMessageIdType)>> myCallbacks;
};


class ManualNetWorkInterface : public NetworkInterface
{
public:
	struct Package
	{
		short myDataLength;
		char myData[NETWORKBUFLEN];
		bool myIsUsingCustomAddress;
		sockaddr myCustomAddress;
	};


	virtual void Receive(char* someData, const int aDataSize) override;
	virtual void Send(const char* someData, const int aDataSize, sockaddr* aCustomAddress = nullptr) override;
	virtual void TimedOut() override;

	bool FetchRecvPackage(Package& aOutPackage);
	bool FetchSendPackage(Package& aOutPackage);

private:
	std::vector<Package> myIncomingPackages;
	std::vector<Package> myOutgoingPackages;
};