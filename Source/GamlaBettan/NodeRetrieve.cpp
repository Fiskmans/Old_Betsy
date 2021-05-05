#include "pch.h"
#include "NodeRetrieve.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeRetrieve::NodeRetrieve()
{
	myPins.push_back(CPin("Key", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Value", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<std::string>(0);
}

int NodeRetrieve::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	std::string key;
	if (!aTriggeringNodeInstance->ReadData(0, key)) { return -1; }

	aTriggeringNodeInstance->WriteData(1, aTriggeringNodeInstance->ourPollingStation->GetSharedStorage(key));

	return -1;
}
