#include "pch.h"
#include "NodeLerp.h"
#include <iostream>
#include "CNodeInstance.h"

Lerp::Lerp()
{
	myPins.push_back(CPin("Val1", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Val2", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("T", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
	SetPinType<float>(2);
	SetPinType<float>(3);
}

int Lerp::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;
	float input2;
	float t;

	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, input2)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(2, t)) { return -1; }

	float temp = input1 * (1-t) + input2 * t;
	if (!aTriggeringNodeInstance->WriteData(3, temp)) { return -1; }

	return -1;
}
