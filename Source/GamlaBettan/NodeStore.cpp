#include "pch.h"
#include "NodeStore.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeStore::NodeStore()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Flow));
	myPins.push_back(CPin("Data", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Name", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	SetPinType<std::string>(3);
}

int NodeStore::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	
	std::any data = aTriggeringNodeInstance->ReadRaw(2);
	std::string key;
	if (!aTriggeringNodeInstance->ReadData(3,key)) { return -1; }
	aTriggeringNodeInstance->ourPollingStation->SetSharedStorage(key, data);
	return 1;
}
