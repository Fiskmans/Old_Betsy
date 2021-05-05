#include "pch.h"
#include "NodeInteractWith.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeInteractWith::NodeInteractWith()
{
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Name for humans", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));


	SetPinType<int>(1);
	SetPinType<std::string>(2);
	SetPinType<GameObjectId>(3);

	SubscribeToMessage(MessageType::EntityInteractedWith);
}

NodeInteractWith::~NodeInteractWith()
{
	UnSubscribeToMessage(MessageType::EntityInteractedWith);
}

int NodeInteractWith::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int ID;

	if (!aTriggeringNodeInstance->ReadData(1, ID)) { return -1; }

	if (ID > 0 && myData.find(ID) == myData.end())
	{
		myData[ID] = aTriggeringNodeInstance;

		for (auto& entity : aTriggeringNodeInstance->ourPollingStation->GetAllEntities())
		{
			if (entity->myInteractableID == ID)
			{
				aTriggeringNodeInstance->WriteData(3, GameObjectId(entity));
				break;
			}
		}
	}

	return -1;
}

void NodeInteractWith::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::EntityInteractedWith && aMessage.myData)
	{
		unsigned int ID = reinterpret_cast<Entity*>(aMessage.myData)->myInteractableID;

		if (myData.find(ID) != myData.end())
		{
			myData[ID]->ExitVia(0);

			SYSINFO("Inctacted with " + std::to_string(ID) + " in node system");
		}
	}
}
