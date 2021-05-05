#include <pch.h>
#include "Connection.h"
#include <SetupMessage.h>
#include <StatusMessage.h>
#include <NetworkHelpers.h>
#include <NetIdentify.h>
#include <DoWorkMessage.h>
#include <sstream>
#include <BigFileMessage.h>
#include <TimeHelper.h>
#include <DeathSpotMessage.h>
#include "GameServer.h"

#define TTL 1.5f

Connection::Connection()
{
	strcpy_s<MAXUSERNAMELENGTH>(myConnectedUser, "[Defaulted/invalid user Profile]");
	myIsValid = false;
	myLastAction = Tools::GetTotalTime();
}

Connection::~Connection()
{
}

Connection::Connection(sockaddr_in aAddress, int aAddressSize, SOCKET aSocket, unsigned short aID, GameServer* aGameServer)
{
	myServer = aGameServer;
	myID = aID;
	mySocket = aSocket;
	myAddress = aAddress;
	myAddressSize = aAddressSize;
	myIsValid = false;
	myLastAction = Tools::GetTotalTime();
	strcpy_s<MAXUSERNAMELENGTH>(myConnectedUser, "[Defaulted/invalid user Profile]");
}

bool Connection::IsAlive()
{
	return myIsValid || (Tools::GetTotalTime() - myLastAction < TTL); // TODO
}

void Connection::Send(const char* aData, int aDataSize, sockaddr* aCustomAddress)
{
	sendto(mySocket, aData, aDataSize, 0, (struct sockaddr*) & myAddress, myAddressSize);
}

void Connection::Receive(char* someData, const int aDataSize)
{
	if (aDataSize == 0)
	{
		std::cout << myConnectedUser << " Has exited gracefully.\n";
		myIsValid = false;
		return;
	}
	myLastAction = Tools::GetTotalTime();
	if (myIsValid)
	{
		Parse(someData, aDataSize);
	}
	else
	{
		myIsValid = HandShake(someData, aDataSize);
	}
}

void Connection::Invalidate()
{
	myIsValid = false;
	NetworkInterface::Clear();
}


void Connection::Flush()
{

}



unsigned short Connection::GetID()
{
	return myID;
}

std::string Connection::GetName()
{
	return myConnectedUser;
}

sockaddr Connection::GetAddress()
{
	return *reinterpret_cast<sockaddr*>(&myAddress);
}

void Connection::AnnounceDeath(DeathMarker aMarker)
{
	DeathSpotMessage death;
	death.myCategory = DeathSpotMessage::Category::Announcement;
	death.myPosition = aMarker.myPosition;
	Send(death);
}



bool Connection::Evaluate(MoveMessage* aMessage)
{
	return true;
}

bool Connection::HandShake(char* aData, int aAmount)
{
	NetMessage* message = reinterpret_cast<NetMessage*>(aData);
	if (message->myType == NetMessage::Type::Setup)
	{
		SetupMessage* setup = reinterpret_cast<SetupMessage*>(aData);
		if (setup->myStep == SetupMessage::SetupStep::Request)
		{
			memset(myConnectedUser, '\0', MAXUSERNAMELENGTH);
			memcpy(myConnectedUser, setup->myIdentifier, min(MAXUSERNAMELENGTH, MAXIDENTIFIERLENGTH));
			myConnectedUser[MAXUSERNAMELENGTH - 1] = '\0';
		}
		else
		{
			std::cout << "Expected setup request got: [" + std::to_string(static_cast<char>(setup->myStep)) + "]\n";
			SetupMessage response;
			response.myResponse.myResult = false;
			NetworkInterface::PreProcessAndSend(&response, sizeof(response));
			return false;
		}
	}
	else
	{
		std::cout << "Expected setup message got: [" + std::to_string(static_cast<short>(message->myType)) + "]\n";
		SetupMessage response;
		response.myResponse.myResult = false;
		NetworkInterface::PreProcessAndSend(&response, sizeof(response));
		return false;
	}

	SetupMessage response;
	response.myResponse.myResult = true;
	response.myResponse.myID = myID;
	response.myStep = SetupMessage::SetupStep::Response;
	NetworkInterface::PreProcessAndSend(&response, sizeof(response));

	StatusMessage logonResponse;
	logonResponse.myAssignedID = myID;
	logonResponse.myUsername = myConnectedUser;
	logonResponse.myStatus = StatusMessage::Status::UserConnected;
	myServer->TransmitMessage(logonResponse);


	std::cout << "Connection Established with user: " << myConnectedUser << " on " << ReadableAddress((sockaddr*)&myAddress) << "\n";

	return true;
}

void Connection::Parse(char* aData, int aAmount)
{
	if (aAmount == SOCKET_ERROR)
	{
		std::cout << "Could not recieve data: " + std::to_string(WSAGetLastError()) + "\n";
		return;
	}

	NetMessage* netMess = reinterpret_cast<NetMessage*>(aData);
	switch (netMess->myType)
	{
	case NetMessage::Type::Status:
	{
		StatusMessage* status = reinterpret_cast<StatusMessage*>(aData);
		if (status->myStatus == StatusMessage::Status::UserDisconnected)
		{
			myServer->TransmitMessage(*status);
		}
		else
		{
			std::cout << myConnectedUser << " behaved badly and was kicked. sent status that was not disconnect\n";
			myIsValid = false;
		}
	}
	break;
	case NetMessage::Type::Identify:
	{
		NetIdentify* ident = reinterpret_cast<NetIdentify*>(netMess);
		switch (ident->myProcessType)
		{
		case NetIdentify::IdentificationType::IsClient:
		{
			NetIdentify response;
			response.myProcessType = NetIdentify::IdentificationType::IsHost;
			Send(response);
			std::cout << GetName() + " identified as a client\n";
		}
		break;
		default:
			std::cout << "Unkown process type connected: " + std::to_string(static_cast<int>(ident->myProcessType)) + "\n";
			break;
		}
	}
	break;
	case NetMessage::Type::Setup:
		break;
	case NetMessage::Type::DeathMessage:
	{
		DeathSpotMessage* death = reinterpret_cast<DeathSpotMessage*>(aData);
		switch (death->myCategory)
		{
		case DeathSpotMessage::Category::Announcement:
		{
			DeathMarker marker;
			marker.myName = GetName();
			marker.myPosition = death->myPosition;
			myServer->LogDeath(this, marker);

			std::cout << GetName() + " died at X:" + std::to_string(death->myPosition.x) + " Y:" + std::to_string(death->myPosition.y) + " Z:" + std::to_string(death->myPosition.z) + "\n";

		}
			break;

		case DeathSpotMessage::Category::Request:
		{
			auto deaths = myServer->GetDeaths();

			std::cout << GetName() + " Requested death points\n";
			std::cout << "Sending " + std::to_string(deaths.size()) + " deaths\n";
			for (auto& i : deaths)
			{
				DeathSpotMessage resp;
				resp.myCategory = DeathSpotMessage::Category::Announcement;
				resp.myPosition = i.myPosition;
				Send(resp);
			}
		}
			break;
		default:
			break;
		}
	}
	break;
	case NetMessage::Type::DoWork:
		break;
	case NetMessage::Type::Invalid:
	default:
		std::cout << myConnectedUser << " behaved badly and was kicked. sent unkown message type: " + std::to_string(static_cast<int>(netMess->myType)) + "\n";
		myIsValid = false;
		return;
	}
}


void Connection::TimedOut()
{
	myIsValid = false;
}
