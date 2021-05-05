#include "pch.h"
#include "NodeAbs.h"
#include <iostream>
#include "CNodeInstance.h"

Abs::Abs()
{
	myPins.push_back(CPin("Val1", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));

	SetPinType<float>(0);
	SetPinType<float>(1);
}

int Abs::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;

	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }

	float temp = abs(input1);
	if (!aTriggeringNodeInstance->WriteData(1, temp)) { return -1; }


	return -1;
}
