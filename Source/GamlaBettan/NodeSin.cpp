#include "pch.h"
#include "NodeSin.h"
#include <iostream>
#include "CNodeInstance.h"

Sin::Sin()
{
	myPins.push_back(CPin("Val1", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
}

int Sin::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;
	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }

	float temp = sin(input1);
	aTriggeringNodeInstance->WriteData(1, temp);

	return -1;
}
