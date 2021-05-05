#include <pch.h>
#include "TClient.h"
#include <iostream>
#include <string>
#include <SetupMessage.h>
#include <thread>
#include <chrono>
#include <functional>
#include <StatusMessage.h>
#include "TimeHelper.h"
#include <NetworkHelpers.h>
#include <NetIdentify.h>
#include <PingMessage.h>
#include <Random.h>
#include <DoWorkMessage.h>


const char* PotentialAddresses[] = {
	SERVERADDRESS,
	LOCAL_HOST
};

NetworkClient::NetworkClient()
{
	Flush();

	myBufferStart = myBuffer;
	myPackageStart = myPackageSize;
}

void NetworkClient::Start()
{

	WSADATA wsa;

	//Initialise winsock
#if USEIMGUI
	Log("Initialising Winsock...");
#endif
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
#if USEIMGUI
		Log("Failed.Error Code: " + std::to_string(WSAGetLastError()));
#endif
		return;
	}
#if USEIMGUI
	Log("Initialised.");
#endif

	//Create a socket
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mySocket == INVALID_SOCKET)
	{
#if USEIMGUI
		Log("Could not create socket: " + std::to_string(WSAGetLastError()));
#endif
		return;
	}
#if USEIMGUI
	Log("Socket created.");
#endif
	memset(&myTargetAddress, 0, sizeof(myTargetAddress));

	u_long mode = 1;  // 1 to enable non-blocking socket
	ioctlsocket(mySocket, FIONBIO, &mode);

	Connect();
}

void NetworkClient::End()
{
	Disconnect();
}



void NetworkClient::Connect()
{
	using namespace std::string_literals;
#if USEIMGUI
	Log("Tranlating: "s + PotentialAddresses[myCurrentAddressAttempt % (sizeof(PotentialAddresses) / sizeof(*PotentialAddresses))]);
#endif
#if USEIMGUI
	TranslateAddress(PotentialAddresses[myCurrentAddressAttempt % (sizeof(PotentialAddresses)/sizeof(*PotentialAddresses))], (sockaddr*)&myTargetAddress, false	, [this](std::string message, bool isError) { if (isError) { Error(message); } else { Log(message); }});
#else
	TranslateAddress(PotentialAddresses[myCurrentAddressAttempt % (sizeof(PotentialAddresses) / sizeof(*PotentialAddresses))], (sockaddr*)&myTargetAddress, false);
#endif
	myTargetAddress.sin_port = htons(SERVERPORT);

#if USEIMGUI
	Log("Final address: "s + ReadableAddress((sockaddr*)&myTargetAddress));
#endif
	HandShake();

	return;
}

void NetworkClient::HandShake()
{
	myIsConnected = false;

	SetupMessage message;
	DWORD length = MAXIDENTIFIERLENGTH;
	GetUserNameA(message.myIdentifier, &length);
	message.myStep = SetupMessage::SetupStep::Request;

	myName = message.myIdentifier.operator const std::string();
#ifdef _DEBUG
	myName += "_Debug";
#endif // _DEBUG
	message.myIdentifier = myName;

#if USEIMGUI
	Log("Starting Handshake " + ReadableAddress((sockaddr*)&myTargetAddress));
	Log("Sending Handshake package");
#endif
	Send(message);
	myIsHandshaking = true;
	myIsWaitingForResponse = true;
}

bool NetworkClient::HandShakeAttempt(char* aData, int aDataSize)
{
	if (aDataSize != sizeof(SetupMessage))
	{
#if USEIMGUI
		Error("Package of wrong size, got [" + std::to_string(aDataSize) + "] expected [" + std::to_string(sizeof(SetupMessage)) + "]");
#endif
		return false;
	}

	NetMessage* netMess = reinterpret_cast<NetMessage*>(aData);
	if (netMess->myType == NetMessage::Type::Setup)
	{
		SetupMessage* setupMess = reinterpret_cast<SetupMessage*>(aData);
		if (setupMess->myResponse.myResult)
		{
#if USEIMGUI
			Log("Handshake Accepted");
			Log("Connection to server established with id: " + std::to_string(setupMess->myResponse.myID) );
#endif
			myId = setupMess->myResponse.myID;

			if (myCallback)
			{
				myCallback(*setupMess);
			}
			myIsConnected = true;

			Message mess;
			mess.myMessageType = MessageType::ConnectedToServer;
			PostMaster::GetInstance()->SendMessages(mess);
		}
		else
		{
#if USEIMGUI
			Error("Server Rejected Handshake");
#endif
		}
	}
	else
	{
#if USEIMGUI
		Error("Got invalid netmessageType: " + std::to_string(static_cast<int>(netMess->myType)));
#endif
		return false;
	}
	myIsHandshaking = false;
	return true;
}

void NetworkClient::TimedOut()
{
#if USEIMGUI
	Error("Server timed out");
#endif
	NetworkInterface::Clear();
#if USEIMGUI
	Log("Reconnecting");
#endif
	myCurrentAddressAttempt++;
	Connect();
}


void NetworkClient::Disconnect()
{
	StatusMessage message;
	message.myAssignedID = myId;
	message.myStatus = StatusMessage::Status::UserDisconnected;
	int slen = sizeof(myTargetAddress);
	bool done = false;
	NetworkInterface::HookCallBack(message.myNetMessageID, [&](NetMessageIdType id) { done = true; });
	PreProcessAndSend(&message,sizeof(message));

	while (!done)
	{
		Flush();
		std::this_thread::yield();
	}
#if USEIMGUI
	Log("Bye-bye! :wave:");
#endif
}

void NetworkClient::SetCallback(std::function<void(NetMessage&)> aFunction)
{
	myCallback = aFunction;
}

void NetworkClient::Flush()
{
	NetworkInterface::Flush();
	sockaddr_in recvAddr;

	if (!myPackageStart)
	{
		myBufferStart = myBuffer;
		myPackageStart = myPackageSize;
		return;
	}

	static size_t dataCount[2] = {};

	if (myBufferStart != myBuffer)
	{
		dataCount[0] += myBufferStart - myBuffer;
	}
	static float lastPrint = Tools::GetTotalTime();
	float now = Tools::GetTotalTime();
	if (now - lastPrint > 1.f)
	{
		lastPrint = now;
		myDataRate[0] = dataCount[0];
		myDataRate[1] = dataCount[1];
		dataCount[0] = 0;
		dataCount[1] = 0;
	}
	short* sizePointer = myPackageSize;
	char* dataPointer = myBuffer;
	while (dataPointer != myBufferStart)
	{
		sendto(mySocket, dataPointer, *sizePointer, 0, reinterpret_cast<sockaddr*>(&myTargetAddress), sizeof(myTargetAddress));
		dataPointer += *sizePointer;
		++sizePointer;
	}
	myBufferStart = myBuffer;
	myPackageStart = myPackageSize;

	char buf[NETWORKBUFLEN];
	int recv_len;
	int slen;
	slen = sizeof(recvAddr);
	while (myIsConnected || (myIsHandshaking  && myIsWaitingForResponse))
	{
		recv_len = recvfrom(mySocket, buf, NETWORKBUFLEN, 0, reinterpret_cast<sockaddr*>(&recvAddr), &slen);
		if (recv_len == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				break;
			}
#if USEIMGUI
			Log("Could not recvieve data from server: " + std::to_string(error), ImColor(1.f, 0.f, 0.f, 1.f));
#endif
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			break;
		}


		dataCount[1] += recv_len;
		if (NetworkInterface::PreProcessReceive(buf, recv_len))
		{
			memcpy(&myRecievedAddress, &recvAddr, sizeof(sockaddr));
			Receive(buf, recv_len);
		}
	}
}

const std::string& NetworkClient::GetName() const
{
	return myName;
}

void NetworkClient::Send(const char* someData, const int aDataSize, sockaddr* aCustomAddress)
{
	if (aCustomAddress)
	{
		sendto(mySocket, someData, aDataSize,0, aCustomAddress, sizeof(*aCustomAddress));
#if USEIMGUI
		Log("Client sent package to custom address: " + ReadableAddress(aCustomAddress));
#endif
	}
	else
	{
		memcpy(myBufferStart, someData, aDataSize);
		*myPackageStart = aDataSize;
		myBufferStart += *myPackageStart;
		++myPackageStart;
	}
}

std::array<char, sizeof(sockaddr)> AddrToArray(sockaddr aAddress)
{
	return *reinterpret_cast<std::array<char, sizeof(sockaddr)>*>(&aAddress);
}

void NetworkClient::EvaluateServer(sockaddr aAddress)
{
#if USEIMGUI
	Log("Evaluating server with ip: " + ReadableAddress(&aAddress));
#endif

	PingMessage message;
	float now = Tools::GetTotalTime();
	NetworkInterface::HookCallBack(message.myNetMessageID, [this, aAddress,now](NetMessageIdType) 
		{ 
			auto key = AddrToArray(aAddress);
			float ping = Tools::GetTotalTime() - now;
			myServerEval[key] = ping;
#if USEIMGUI
			Log("Server pinged in " + std::to_string(ping) + " seconds");
#endif
			StatusMessage response;
			response.myStatus = StatusMessage::Status::EvaluatedServer;
			response.myAddress = aAddress;
			response.myEvalServer.aPing = ping;
			Send(response);
		});
	Send(message,&aAddress);
}

void NetworkClient::Receive(char* someData, const int aDataSize)
{
	if (myIsHandshaking && HandShakeAttempt(someData, aDataSize))
	{
	}
	else
	{
		NetMessage* netMess = reinterpret_cast<NetMessage*>(someData);

		if (netMess->myType == NetMessage::Type::Status)
		{
			StatusMessage* status = reinterpret_cast<StatusMessage*>(netMess);
			if (status->myStatus == StatusMessage::Status::PotentialServer)
			{
				EvaluateServer(status->myAddress);
			}
			if (status->myStatus == StatusMessage::Status::ConnectToServer)
			{
#if USEIMGUI
				Log("Redirecting to: " + ReadableAddress(&status->myAddress));
#endif

				memcpy(&myTargetAddress, &status->myAddress, sizeof(sockaddr));
				HandShake();
			}
		}
		if (netMess->myType == NetMessage::Type::Setup)
		{
			SetupMessage* setupMessage = reinterpret_cast<SetupMessage*>(someData);
#if USEIMGUI
			Log("Connection invalidated!",ImColor(1.f,0.f,0.f,1.f));
			Log("Reconnecting", ImColor(1.f, 1.f, 0.f, 1.f));
#endif
			HandShake();
		}
		else if (netMess->myType == NetMessage::Type::Identify)
		{
			NetIdentify* ident = reinterpret_cast<NetIdentify*>(netMess);
			switch (ident->myProcessType)
			{
			case NetIdentify::IdentificationType::IsServer:
#if USEIMGUI
				Log("connected to server");
#endif
				break;
			case NetIdentify::IdentificationType::IsHost:
#if USEIMGUI
				Log("connected to host");
#endif
				break;
			default:
#if USEIMGUI
				Log("unexpected server type!",ImColor(1.f,0.f,0.f,1.f));
#endif
				break;
			}
		}
		else if (myCallback)
		{
			myCallback(*netMess);
		}
	}
}
#if USEIMGUI
void NetworkClient::ImGui()
{
	const char* modifiers[] = { "b","mb","gb","tb","eb" };

	{
		size_t down = myDataRate[1];
		int steps = 0;
		for (size_t i = 0; i < sizeof(modifiers) / sizeof(*modifiers); i++)
		{
			if (down > 1024)
			{
				down /= 1024;
				++steps;
			}
		}
		ImGui::Text("Down:%zu%s/s",down,modifiers[steps]);
	}

	{
		size_t up = myDataRate[0];
		int steps = 0;
		for (size_t i = 0; i < sizeof(modifiers) / sizeof(*modifiers); i++)
		{
			if (up > 1024)
			{
				up /= 1024;
				++steps;
			}
		}
		ImGui::Text("Up:%zu%s/s", up, modifiers[steps]);
	}
	if (ImGui::BeginChild("Log",ImVec2(500,300),true))
	{
		for (auto& i : myMessages)
		{
			ImGui::TextColored(i.second, i.first.c_str());
		}
	}
	ImGui::EndChild();
}

inline void NetworkClient::Log(std::string aMessage, ImColor aColor)
{
	myMessages.push_back(std::make_pair(aMessage, aColor));
}

inline void NetworkClient::Error(std::string aMessage, ImColor aColor)
{
	Log(aMessage, aColor);
}
#endif
