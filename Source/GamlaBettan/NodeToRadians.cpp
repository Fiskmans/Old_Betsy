#include "pch.h"
#include "NodeToRadians.h"
#include <iostream>
#include "CNodeInstance.h"

ToRadians::ToRadians()
{
	myPins.push_back(CPin("Val", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
}

int ToRadians::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;
	if (!aTriggeringNodeInstance->ReadData(1, input1)) { return -1; }

	float temp = input1 * 0.01745329251f;
	aTriggeringNodeInstance->WriteData(1, temp);

	return -1;
}
