#include "pch.h"
#include "NodeWithinRangeOfSeekingFriends.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "..//Game/AIController.h"
#include "..//Game/Entity.h"
#include "..//Game/AIPollingStation.h"

NodeWithinRangeOfSeekingFreinds::NodeWithinRangeOfSeekingFreinds()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Range", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("True", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("False", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<float>(1);
	SetPinType<int>(4);
}

int NodeWithinRangeOfSeekingFreinds::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	float range = 0;

	if (!aTriggeringNodeInstance->ReadData(4, id)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, range)) { return -1; }

	if (id.myInteral)
	{
		AIController* ai = id.myInteral->GetComponent<AIController>();

		if (ai)
		{
			V3F distance;
			ai->withinRangeOfPlayer = false;
			for (Entity* aiFriend : ai->GetPollingStation()->GetSeekingEnemies())
			{
				AIController* component = aiFriend->GetComponent<AIController>();

				if (component && !component->withinRangeOfPlayer)
				{
					return 3;
				}

				distance = aiFriend->GetPosition() - id.myInteral->GetPosition();
				if (distance.LengthSqr() < range * range)
				{
					return 2;
				}
			}
		}
	}


	return 3;
}
