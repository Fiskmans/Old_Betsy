#include "pch.h"
#include "NodeSetTargetPosition.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"

NodeSetTargetPosition::NodeSetTargetPosition()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Position", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));

	SetPinType<GameObjectId>(2);
	SetPinType<V3F>(3);
}

int NodeSetTargetPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	V3F position;
	if (!aTriggeringNodeInstance->ReadData(2, id)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, position)) { return -1; }
	if (id.myInteral)
	{
		id.myInteral->SetTargetPosition(position);
	}
	else
	{
		LOGWARNING("SetTargetPosition node got invalid id");
	}

	return 1;
}
