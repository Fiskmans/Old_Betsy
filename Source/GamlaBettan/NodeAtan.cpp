#include "pch.h"
#include "NodeAtan.h"
#include <iostream>
#include "CNodeInstance.h"

Atan::Atan()
{
	myPins.push_back(CPin("Val1", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Val2", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
	SetPinType<float>(2);
}

int Atan::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;
	float input2;

	if (!aTriggeringNodeInstance->ReadData(0,input1)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, input2)) { return -1; }

	float temp = atan2(input1,input2);
	if (!aTriggeringNodeInstance->WriteData(2, temp)) { return -1; }

	return -1;
}
