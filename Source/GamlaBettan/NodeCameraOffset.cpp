#include "pch.h"
#include "NodeCameraOffset.h"
#include <iostream>
#include "CNodeInstance.h"

NodeCameraOffset::NodeCameraOffset()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));

	myPins.push_back(CPin("Offset Position", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<V3F>(1);
}

int NodeCameraOffset::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	V3F offsetPos;

	if (!aTriggeringNodeInstance->ReadData(1, offsetPos)) { return -1; }

	Message message;
	message.myMessageType = MessageType::SetCameraOffset;
	message.myData = &offsetPos;
	SendMessages(message);

	return -1;
}