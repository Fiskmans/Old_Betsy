#include <pch.h>
#include "GameServer.h"
#include <string>
#include <SetupMessage.h>
#include <StatusMessage.h>
#include <thread>
#include <TimeHelper.h>
#include <Random.h>
#include <NetworkHelpers.h>
#include <NetIdentify.h>

#define LOADTOOFFLOADAT 0.8f

GameServer::GameServer()
{
	myLastRapportTime = Tools::GetTotalTime();
}


GameServer::~GameServer()
{
}


int CalculateBroadcastIP(std::string aIP, std::string aSubNet, std::string& aBroadcast)
{
	struct in_addr host, mask, broadcast;
	char broadcast_address[INET_ADDRSTRLEN];
	if (inet_pton(AF_INET, aIP.c_str(), &host) == 1 &&
		inet_pton(AF_INET, aSubNet.c_str(), &mask) == 1)
	{
		broadcast.S_un.S_addr = host.S_un.S_addr | ~mask.S_un.S_addr;
	}
	else
	{
		return 1;
	}
	if (inet_ntop(AF_INET,&broadcast,broadcast_address,INET_ADDRSTRLEN) != NULL)
	{
		std::cout << "Broadcast address of " + aIP + " with netmask " + aSubNet + " is " + broadcast_address + "\n";
	}
	else
	{
		return 1;
	}

	aBroadcast = broadcast_address;
	return S_OK;
}

void GameServer::StartServer()
{
	struct sockaddr_in si_other;
	int slen, recv_len;
	char buf[NETWORKBUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	std::cout << "Initialising Winsock...\n";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Failed.Error Code: " + std::to_string(WSAGetLastError()) + "\n";
		return;
	}
	std::cout << "Initialised.\n";

	//Create a socket
	mySocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (mySocket == INVALID_SOCKET)
	{
		std::cout << "Could not create socket: " + std::to_string(WSAGetLastError()) + "\n";
		return;
	}
	std::cout << "Socket created.\n";

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int iResult = getaddrinfo(NULL, STRINGVALUE(SERVERPORT), &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return;
	}

	std::cout << "Got address info!\n";

	//Bind
	sockaddr bindAddress = *result->ai_addr;
	((sockaddr_in*)&bindAddress)->sin_port = htons(SERVERPORT);
	if (bind(mySocket, &bindAddress, sizeof(bindAddress)))
	{
		std::cout << "Failed to bind to: " + std::to_string(SERVERPORT) + "\n";
		return;
	}

	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(mySocket, (struct sockaddr*) & sin, &len) != SOCKET_ERROR)
	{
		std::cout << "Bind Successfull on ip: " + ReadableAddress((sockaddr*)&sin) + "\n";
	}
	else
	{
		std::cout << "could not retrieve address from bound socket but got no error on bind.\n";
	}

	u_long mode = 1;  // 1 to enable non-blocking socket
	ioctlsocket(mySocket, FIONBIO, &mode);
	
	LoadDeaths();
	Listen();

	closesocket(mySocket);
	WSACleanup();

	return;
}



void GameServer::Listen()
{
	sockaddr_in si_other;
	si_other.sin_family = AF_INET;
	int slen, recv_len;
	char buf[NETWORKBUFLEN];
	slen = sizeof(si_other);

	sockaddr sin;
	int len = sizeof(sin);
	if (getsockname(mySocket,& sin, &len) != SOCKET_ERROR)
	{
		std::cout << "Listening on: " + ReadableAddress(&sin) + "\n";
	}
	else
	{
		std::cout << "Could not get address info on listener\n";
	}

	while (true)
	{
		Flush();

		for (auto& cli : myClients)
		{
			if (!cli.second.IsAlive())
			{
				StatusMessage message;
				message.myStatus = StatusMessage::Status::UserDisconnected;
				message.myAssignedID = cli.second.GetID();
				TransmitMessage(message);
				myClients.erase(cli.first);
				break;
			}
			else
			{
				cli.second.Flush();
			}
		}

		recv_len = recvfrom(mySocket, buf, NETWORKBUFLEN, 0, reinterpret_cast<sockaddr*>(&si_other), &slen);
		
		std::string key = ReadableAddress((sockaddr*)&si_other);
		if (recv_len == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				std::this_thread::yield();
				continue;
			}
			if (error != 0)
			{
				std::cout << "Could not recvieve data from client(s): " + std::to_string(error) + " ip: " + key + "\n";
				myClients[key].Invalidate();
			}
			continue;
		}

		auto it = myClients.find(key);
		if (it == myClients.end())
		{
			Connection* expected = nullptr;
			for (size_t i = 0; i < myIncomingConnections.size(); i++)
			{
				if (ReadableAddress(&myIncomingConnections[i]->GetAddress()) == key)
				{
					expected = myIncomingConnections[i];
					myIncomingConnections.erase(myIncomingConnections.begin() + i);
					break;
				}
			}
			if (expected)
			{
				std::cout << "Expected user: " + expected->GetName() + " Connected\n";
				myClients[key] = *expected;
				delete expected;
			}
			else
			{
				myClients[key] = Connection(si_other, slen, mySocket, ++myConnectionIdCounter,this);
			}

			it = myClients.find(key);
		}

		if (it->second.PreProcessReceive(buf, recv_len))
		{
			it->second.Receive(buf, recv_len);
		}
	}
}

void GameServer::TransmitMessage(const NetMessage& aMessage)
{
	switch (aMessage.myType)
	{
	case NetMessage::Type::Status:
	{
		const StatusMessage& status = *reinterpret_cast<const StatusMessage*>(&aMessage);
		switch (status.myStatus)
		{
		case StatusMessage::Status::UserConnected:
		{
			std::cout << "Telling all clients that [" << status.myUsername << "] has connected on id [" + std::to_string(status.myAssignedID) + "]\n";
			for (auto& it : myClients)
			{
				if (it.second.GetID() == status.myAssignedID)
				{
					for (auto& cli : myClients)
					{
						if (cli.second.GetID() != it.second.GetID())
						{
							StatusMessage userStatus;
							userStatus.myUsername = cli.second.GetName();
							userStatus.myStatus = StatusMessage::Status::UserOnline;
							userStatus.myAssignedID = cli.second.GetID();
							it.second.Send(userStatus);
						}
					}
				}
				else
				{
					it.second.Send(status);
				}
			}
		}
		break;
		case StatusMessage::Status::UserDisconnected:

		{
			std::cout << "Telling all clients that [";
			for (auto& i : myClients)
			{
				if (i.second.GetID() == status.myAssignedID)
				{
					std::cout << i.second.GetName();
				}
			}
			std::cout << "] has disconnected.\n";
			for (auto& it : myClients)
			{
				if (it.second.GetID() != status.myAssignedID)
				{
					it.second.Send(status);
				}
			}
		}
		break;
		case StatusMessage::Status::UserOnline:
		default:
			std::cout << "This should never happen\n";
			break;
		}
	}
	break;
	default:
		break;
	}

}


void GameServer::LoadDeaths()
{
	std::ifstream file;
	file.open("ServerData.db");
	DeathMarker buffer;
	std::string name;

	while (file >> name >> buffer.myPosition.x >> buffer.myPosition.y >> buffer.myPosition.z)
	{
		buffer.myName = name;
		myDeaths.push_back(buffer);
		std::cout << "Loaded death of " + buffer.myName.operator const std::string() + " At X:" + std::to_string(buffer.myPosition.x) + " Y:" + std::to_string(buffer.myPosition.y) + " Z:" + std::to_string(buffer.myPosition.z) + "\n";
	}
}

void GameServer::LogDeath(Connection* aConnector, DeathMarker aMarker)
{
	myDeaths.push_back(aMarker);
	for (auto& i : myClients)
	{
		if (aConnector != &i.second || true)
		{
			i.second.AnnounceDeath(aMarker);
		}
	}
	std::ofstream file;
	file.open("ServerData.db",std::ios::ate |std::ios::app | std::ios::out);
	std::string name = aMarker.myName.operator const std::string();
	name = name.substr(0, name.find(" "));
	file << name << " " << aMarker.myPosition.x << " " << aMarker.myPosition.y << " " << aMarker.myPosition.z << std::endl;
	file.close();
}

std::vector<DeathMarker> GameServer::GetDeaths()
{
	return myDeaths;
}

void GameServer::Flush()
{

}
