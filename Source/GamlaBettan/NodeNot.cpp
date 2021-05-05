#include "pch.h"
#include "NodeNot.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

Not::Not()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("out", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<bool>(0);
	SetPinType<bool>(1);
}

int Not::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	bool input1;
	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }

	bool temp = !input1;
	aTriggeringNodeInstance->WriteData(1, temp);

	return -1;
}
