#include "pch.h"
#include "NodeSetObjectScale.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"
#include "../Game/Mesh.h"

SetObjectScale::SetObjectScale()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("Id", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Scale", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<GameObjectId>(2);
	SetPinType<V3F>(3);
}

int SetObjectScale::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	if (!aTriggeringNodeInstance->ReadData(2,id)) { return -1; }

	V3F scale;
	if (!aTriggeringNodeInstance->ReadData(3, scale)) { return -1; }

	if (id.myInteral)
	{
		id.myInteral->SetScale(scale);
	}
	else
	{
		SYSWARNING("Set scale node got invalid id","");
	}




	return 1;
}
