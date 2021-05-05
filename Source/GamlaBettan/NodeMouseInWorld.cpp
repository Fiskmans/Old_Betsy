#include "pch.h"
#include "NodeMouseInWorld.h"
#include <iostream>
#include "CNodeInstance.h"

MouseInWorld::MouseInWorld()
{
	myPins.push_back(CPin("X", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	myPins.push_back(CPin("Y", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
}

int MouseInWorld::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	return -1;
}
