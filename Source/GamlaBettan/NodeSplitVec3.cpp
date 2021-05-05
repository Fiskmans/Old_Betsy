#include "pch.h"
#include "NodeSplitVec3.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

SplitVec3::SplitVec3()
{
	myPins.push_back(CPin("Input", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("X", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	myPins.push_back(CPin("Y", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	myPins.push_back(CPin("Z", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<V3F>(0);
	SetPinType<float>(1);
	SetPinType<float>(2);
	SetPinType<float>(3);
}

int SplitVec3::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	V3F pos;
	if (!aTriggeringNodeInstance->ReadData(0, pos)) { return -1; }

	aTriggeringNodeInstance->WriteData(1, pos.x);
	aTriggeringNodeInstance->WriteData(2, pos.y);
	aTriggeringNodeInstance->WriteData(3, pos.z);

	return -1;
}
