#include "pch.h"
#include "NodeLengthSqr.h"
#include <iostream>
#include "CNodeInstance.h"

LengthSqr::LengthSqr()
{
	myPins.push_back(CPin("Start", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("End", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));

	SetPinType<V3F>(0);
	SetPinType<V3F>(1);
	SetPinType<float>(2);
}

int LengthSqr::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	V3F start, end;
	if (!aTriggeringNodeInstance->ReadData(0, start)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, end)) { return -1; }


	float result = (start-end).LengthSqr();
	aTriggeringNodeInstance->WriteData(2, result);
	return -1;
}
