#include "pch.h"
#include "NodeCNodeTypeStart.h"
#include <iostream>
#include "CNodeType.h"
#include "CNodeInstance.h"


CNodeTypeStart::CNodeTypeStart()
{
	myPins.push_back(CPin("Updating", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("", CPin::PinTypeInOut::PinTypeInOut_OUT));

	SetPinType<bool>(0);
}

int CNodeTypeStart::OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	if (!aTriggeringNodeInstance->ReadData(0, aTriggeringNodeInstance->myShouldTriggerAgain)) { return -1; }
	return 1;
}
