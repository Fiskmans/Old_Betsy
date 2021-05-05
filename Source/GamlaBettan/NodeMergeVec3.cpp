#include "pch.h"
#include "NodeMergeVec3.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

MergeVec3::MergeVec3()
{
	myPins.push_back(CPin("X", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Y", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Z", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Vec3", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
	SetPinType<float>(2);
	SetPinType<V3F>(3);
}

int MergeVec3::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float x, y, z;
	if (!aTriggeringNodeInstance->ReadData(0, x)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, y)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(2, z)) { return -1; }

	aTriggeringNodeInstance->WriteData(3, V3F(x, y, z));

	return -1;
}
