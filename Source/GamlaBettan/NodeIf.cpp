#include "pch.h"
#include "NodeIf.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

If::If()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Flow));
	myPins.push_back(CPin("If", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Flow));
	myPins.push_back(CPin("Else", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("Condition", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<bool>(3);
}

int If::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	bool condition;
	if (!aTriggeringNodeInstance->ReadData(3, condition)) { return -1; }

	if (condition)
	{
		return 1;
	}
	else
	{
		return 2;
	}

	LOGVERBOSE("If bad");
	return -1;
}
