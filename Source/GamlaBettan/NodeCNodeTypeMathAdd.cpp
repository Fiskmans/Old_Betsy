#include "pch.h"
#include "NodeCNodeTypeMathAdd.h"
#include "CNodeInstance.h"

CNodeTypeMathAdd::CNodeTypeMathAdd()
{
	myPins.push_back(CPin("Val1", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Val2", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("OUT", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));

	SetPinType<float>(0);
	SetPinType<float>(1);
	SetPinType<float>(2);
}

int CNodeTypeMathAdd::OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float input1;
	float input2;

	if (!aTriggeringNodeInstance->ReadData(0, input1)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, input2)) { return -1; }

	float temp = input1 + input2;
	if (!aTriggeringNodeInstance->WriteData(2, temp)) { return -1; }

	return -1;
}
