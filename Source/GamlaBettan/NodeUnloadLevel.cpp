#include "pch.h"
#include "NodeUnloadLevel.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeUnloadLevel::NodeUnloadLevel()
{
	myPins.push_back(CPin("IN", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("File name", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	SetPinType<std::string>(2);
}

int NodeUnloadLevel::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	std::string input1;
	if (!aTriggeringNodeInstance->ReadData(2, input1)) { return -1; }
	Message loadmessage;
	loadmessage.myMessageType = MessageType::UnloadLevel;
	loadmessage.myText = input1;
	PostMaster::GetInstance()->SendMessages(loadmessage);

	return 2;
}
