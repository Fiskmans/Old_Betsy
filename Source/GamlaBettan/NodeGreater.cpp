#include "pch.h"
#include "NodeGreater.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeGreater::NodeGreater()
{
	myPins.push_back(CPin("Left", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Right", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<float>(0);
	SetPinType<float>(1);
	SetPinType<bool>(2);
}

int NodeGreater::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;
	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }
	float input2;
	if (!aTriggeringNodeInstance->ReadData(1, input2)) { return -1; }

	bool temp = input1 > input2;
	LOGVERBOSE(std::to_string(input1) + " > " + std::to_string(input2) + " = " + std::to_string(temp));
	aTriggeringNodeInstance->WriteData(2, temp);

	return -1;
}
