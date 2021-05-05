#include "pch.h"
#include "NodeFloor.h"
#include <iostream>
#include "CNodeInstance.h"

Floor::Floor()
{
	myPins.push_back(CPin("Val", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
}

int Floor::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;
	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }

	float temp = floor(input1);
	aTriggeringNodeInstance->WriteData(1, temp);

	return -1;
}
