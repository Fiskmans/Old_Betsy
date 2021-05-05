#pragma once
#include <CommonNetworkIncludes.h>
#include <thread>
#include <string>
#include <unordered_map>
#include <functional>
#include <unordered_map>
#include <NetworkInterface.h>
#include <future>
#include <variant>
#include <vector>

#define MAXBUFFERSIZE 1024 << 5
#define MAXBUFFERCOUNT 1024

class NetMessage;
class NetworkClient : NetworkInterface
{
public:
	NetworkClient();

	void Start();
	void End();

	template<class T>
	void Send(const T& aMessage,sockaddr* aCustomAddress = nullptr);

	void SetCallback(std::function<void(NetMessage&)> aFunction);
	void Flush();

	const std::string& GetName() const;


	void Receive(char* someData, const int aDataSize) override;
	void Send(const char* someData, const int aDataSize, sockaddr* aCustomAddress = nullptr) override;

#if USEIMGUI
	void ImGui();
#endif

private:
	void Connect();
	void Disconnect();

#if USEIMGUI
	void Log(std::string aMessage,ImColor aColor = ImColor(1.f,1.f,1.f,1.f));
	void Error(std::string aMessage, ImColor aColor = ImColor(1.f, 0.f, 0.f, 1.f));
#endif
	void EvaluateServer(sockaddr aAddress);
	std::map<std::array<char,sizeof(sockaddr)>,float> myServerEval;

	float myLastServerLoad;
	float myLoadOnServer;

	char myBuffer[MAXBUFFERSIZE];
	short myPackageSize[MAXBUFFERCOUNT];
	char* myBufferStart = nullptr;
	short* myPackageStart = nullptr;

	SOCKET mySocket;
	sockaddr_in myTargetAddress;
	sockaddr_in myRecievedAddress;


	void HandShake();
	bool HandShakeAttempt(char* aData,int aDataSize);

	std::function<void(NetMessage&)> myCallback;
	std::string myName;

	unsigned short myId;
	bool myIsHandshaking = false;
	bool myIsWaitingForResponse = false;
	bool myIsConnected = false;

	size_t myDataRate[2];
	size_t myCurrentAddressAttempt = 0;

#if USEIMGUI
	std::vector<std::pair<std::string, ImColor>> myMessages;
#endif
	// Inherited via NetworkInterface
	virtual void TimedOut() override;
};

template<class T>
inline void NetworkClient::Send(const T& aMessage, sockaddr* aCustomAddress)
{
	NetworkInterface::PreProcessAndSend(const_cast<NetMessage*>(reinterpret_cast<const NetMessage*>(&aMessage)), sizeof(aMessage),aCustomAddress);
}
