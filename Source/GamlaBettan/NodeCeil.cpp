#include "pch.h"
#include "NodeCeil.h"
#include <iostream>
#include "CNodeInstance.h"

Ceil::Ceil()
{
	myPins.push_back(CPin("Val", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
}

int Ceil::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;

	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }

	float temp = ceil(input1);
	if (!aTriggeringNodeInstance->WriteData(2, temp)) { return -1; }


	return -1;
}
