#include "pch.h"
#include "NodeDialogue.h"
#include <iostream>
#include "CNodeInstance.h"

NodeDialogue::NodeDialogue()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("Dialogue path", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	myPins.push_back(CPin("Dialogue path", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));

	SetPinType<std::string>(2);
	SetPinType<GameObjectId>(3);
	SetPinType<GameObjectId>(4);
	SetPinType<std::string>(5);
}

int NodeDialogue::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	std::string path;

	if (!aTriggeringNodeInstance->ReadData(2, path)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, id)) { return -1; }
	aTriggeringNodeInstance->WriteData(4, id);
	aTriggeringNodeInstance->WriteData(5, path);

	//if (id.myInteral)
	{
		path = "Data/Dialogues/" + path + ".json";

		Message message;
		message.myMessageType = MessageType::StartDialogue;
		message.myText = path;
		message.myData = id.myInteral;
		PostMaster::GetInstance()->SendMessages(message);
	}

	return 2;
}