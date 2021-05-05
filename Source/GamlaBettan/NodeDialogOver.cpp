#include "pch.h"
#include "NodeDialogOver.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeDialogOver::NodeDialogOver()
{
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Dialog path", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Conversation num", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));


	SetPinType<GameObjectId>(1);
	SetPinType<std::string>(2);
	SetPinType<int>(3);
	SetPinType<GameObjectId>(4);

	SubscribeToMessage(MessageType::DialogueStarted);
	SubscribeToMessage(MessageType::DialogueOver);

	myQueuedNode = nullptr;
}

NodeDialogOver::~NodeDialogOver()
{
	UnSubscribeToMessage(MessageType::DialogueStarted);
	UnSubscribeToMessage(MessageType::DialogueOver);
}

int NodeDialogOver::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId ID;
	std::string info;
	int num;

	if (!aTriggeringNodeInstance->ReadData(1, ID)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, num)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(2, info)) { return -1; }

	info = std::to_string(num) + "Data/Dialogues/" + info + ".json";

	aTriggeringNodeInstance->WriteData(4, ID);

	if (myData.find(info) == myData.end())
	{
		myData[info] = aTriggeringNodeInstance;
	}

	return -1;
}

void NodeDialogOver::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::DialogueStarted)
	{
		std::string info;
		info = std::to_string(aMessage.myIntValue) + aMessage.myText.data();

		if (myData.find(info) != myData.end())
		{
			myQueuedNode = myData[info];
		}
	}
	else if (aMessage.myMessageType == MessageType::DialogueOver)
	{
		if (myQueuedNode)
		{
			myQueuedNode->ExitVia(0);
			myQueuedNode = nullptr;
		}
	}
}
