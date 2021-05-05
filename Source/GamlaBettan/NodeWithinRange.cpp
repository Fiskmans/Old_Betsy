#include "pch.h"
#include "NodeWithinRange.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

WithinRange::WithinRange()
{
	myPins.push_back(CPin("Start", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("End", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Range", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<V3F>(0);
	SetPinType<V3F>(1);
	SetPinType<float>(2);
	SetPinType<bool>(3);
}

int WithinRange::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	V3F start,end;
	if (!aTriggeringNodeInstance->ReadData(0, start)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, end)) { return -1; }
	float range;
	if (!aTriggeringNodeInstance->ReadData(2, range)) { return -1; }

	bool result = (start - end).LengthSqr() < range * range;
	aTriggeringNodeInstance->WriteData(3, result);

	return -1;
}
