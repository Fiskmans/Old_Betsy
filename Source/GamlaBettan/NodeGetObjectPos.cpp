#include "pch.h"
#include "NodeGetObjectPos.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"

GetObjectPos::GetObjectPos()
{
	myPins.push_back(CPin("Id", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Pos", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<GameObjectId>(0);
	SetPinType<V3F>(1);
}

int GetObjectPos::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	//CPin::PinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GameObjectId id;
	if (!aTriggeringNodeInstance->ReadData(0, id)) { return -1; }
	


	V3F Position;
	if (id.myInteral)
	{
		Position = id.myInteral->GetPosition();
	}
	aTriggeringNodeInstance->WriteData(1, Position);

	return -1;
}
