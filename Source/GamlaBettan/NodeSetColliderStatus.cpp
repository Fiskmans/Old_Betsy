#include "pch.h"
#include "NodeSetColliderStatus.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"
#include "../Game/Collision.h"

NodeSetColliderStatus::NodeSetColliderStatus()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT));

	myPins.push_back(CPin("Id", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Active", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<int>(2);
	SetPinType<bool>(3);
	SetPinType<float>(4);
}

int NodeSetColliderStatus::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	bool isActive;

	if (!aTriggeringNodeInstance->ReadData(2, id)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, isActive)) { return -1; }

	if (id.myInteral)
	{
		if (id.myInteral->GetComponent<Collision>())
		{
			id.myInteral->GetComponent<Collision>()->SetIsActive(isActive);
		}
		else
		{
			SYSERROR("Entity does not have a collider!");
		}
	}

	return 1;
}
