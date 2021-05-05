#include "pch.h"
#include "NodeOnTrigger.h"
#include <iostream>
#include "CNodeType.h"
#include "CNodeInstance.h"

NodeOnTrigger::NodeOnTrigger()
{
	//myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(CPin("EnterOnce", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("OnEnter", CPin::PinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(CPin("ExitOnce", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("OnExit", CPin::PinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(CPin("EventID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("ActiveObjectID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<bool>(0);
	SetPinType<bool>(2);
	SetPinType<int>(4);
	SetPinType<EntityType>(5);

	SubscribeToMessage(MessageType::FadeInComplete);
}

NodeOnTrigger::~NodeOnTrigger()
{
	UnSubscribeToMessage(MessageType::TriggerEvent);
}

int NodeOnTrigger::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int triggerID;
	if (!aTriggeringNodeInstance->ReadData(4, triggerID)) { return -1; }

	if (myTriggerStructs.count(triggerID) > 0)
	{
		SAFE_DELETE(myTriggerStructs[triggerID]);
		myTriggerStructs.erase(triggerID);
	}

	TriggerStruct* triggerStruct = new TriggerStruct();
	triggerStruct->aNodeInstance = aTriggeringNodeInstance;
	aTriggeringNodeInstance->ReadData(0, triggerStruct->aEnterOnce);
	aTriggeringNodeInstance->ReadData(2, triggerStruct->aExitOnce);
	aTriggeringNodeInstance->ReadData(5, triggerStruct->aFilteredTypeID);
	myTriggerStructs[triggerID] = triggerStruct;

	return -1;
}

void NodeOnTrigger::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::FadeInComplete)
	{
		SubscribeToMessage(MessageType::TriggerEvent);
	}

	if (aMessage.myMessageType == MessageType::TriggerEvent)
	{
		TriggerStruct* currentStruct = myTriggerStructs[aMessage.myIntValue];

		if (currentStruct)
		{
			if (currentStruct->aFilteredTypeID == CAST(EntityType, aMessage.myIntValue2))
			{
				//ON ENTER
				if (aMessage.myBool)
				{
					if (currentStruct->aFirstEnter)
					{
						currentStruct->aFirstEnter = false;
					}
					else
					{
						if (currentStruct->aEnterOnce)
						{
							return;
						}
					}
					currentStruct->aNodeInstance->ExitVia(1);
				}
				//ON EXIT
				else
				{
					if (currentStruct->aFirstExit)
					{
						currentStruct->aFirstExit = false;
					}
					else
					{
						if (currentStruct->aExitOnce)
						{
							return;
						}
					}
					currentStruct->aNodeInstance->ExitVia(3);
				}
			}
		}
	}
}
