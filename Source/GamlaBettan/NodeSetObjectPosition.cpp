#include "pch.h"
#include "NodeSetObjectPosition.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"

SetObjectPosition::SetObjectPosition()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT));

	myPins.push_back(CPin("Id", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Pos", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));

	SetPinType<int>(2);
	SetPinType<V3F>(3);
	SetPinType<float>(4);
}

int SetObjectPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	V3F pos;

	if (!aTriggeringNodeInstance->ReadData(2, id)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, pos)) { return -1; }

	if (id.myInteral)
	{
		id.myInteral->SetPosition(pos);
	}

	return 1;
}
