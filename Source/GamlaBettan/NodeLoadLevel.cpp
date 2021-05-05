#include "pch.h"
#include "NodeLoadLevel.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeLoadLevel::NodeLoadLevel()
{
	myPins.push_back(CPin("IN", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("File name", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	SetPinType<std::string>(2);
}

int NodeLoadLevel::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	std::string input1;
	if (!aTriggeringNodeInstance->ReadData(2, input1)) { return -1; }
	Message loadmessage;
	loadmessage.myMessageType = MessageType::LoadLevel;
	loadmessage.myText = input1;
	PostMaster::GetInstance()->SendMessages(loadmessage);

	return 2;
}
