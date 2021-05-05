#include "pch.h"
#include "NodeSeekTarget.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"
#include "../Game/AIController.h"

NodeSeekTarget::NodeSeekTarget()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT));

	myPins.push_back(CPin("Seeking ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<int>(2);
}

int NodeSeekTarget::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	assert(false && L"AI-Node not supported");

	//GameObjectId id;

	//if (!aTriggeringNodeInstance->ReadData(2, id)) { return -1; }

	//if (id.myInteral)
	//{
	//	AIController* ai = id.myInteral->GetComponent<AIController>();

	//	if (ai)
	//	{
	//		ai->SetState(AIController::AIStates::SeekTarget);
	//		ai->withinRangeOfPlayer = true;
	//	}
	//}

	return 1;
}