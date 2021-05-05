#include "pch.h"
#include "NodeSetTarget.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"
#include "../Game/AIController.h"

NodeSetTarget::NodeSetTarget()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT));

	myPins.push_back(CPin("Id", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Id Target", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<int>(2);
	SetPinType<int>(3);
}

int NodeSetTarget::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	assert(false && L"AI node not supported");
	/*GameObjectId id;
	GameObjectId targetId;

	if (!aTriggeringNodeInstance->ReadData(2, id)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, targetId)) { return -1; }

	if (id.myInteral)
	{
		AIController* ai = id.myInteral->GetComponent<AIController>();
		Entity* target = targetId.myInteral;

		if (ai && target)
		{
			ai->SetTarget(target);
		}
	}*/

	return 1;
}