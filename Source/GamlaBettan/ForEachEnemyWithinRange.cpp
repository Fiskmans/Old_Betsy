#include "pch.h"
#include "ForEachEnemyWithinRange.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"
#include "..//Game/AIController.h"
#include "..//Game/AnimationComponent.h"

ForEachEnemyWithinRange::ForEachEnemyWithinRange()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("OnIter", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	myPins.push_back(CPin("Range", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<GameObjectId>(3);
	SetPinType<float>(4);
}

int ForEachEnemyWithinRange::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	assert(false && L"AI-Node not supported");

	/*float range;
	if (!aTriggeringNodeInstance->ReadData(4, range)) { return -1; }
	std::vector<class Entity*> entities = aTriggeringNodeInstance->ourPollingStation->GetFiltered(EntityType::Enemy);

	for (auto& i : entities)
	{
		if ((i->GetPosition() - aTriggeringNodeInstance->ourPollingStation->GetPlayer()->GetPosition()).LengthSqr() < (range * range))
		{
			if (i->GetComponent<AnimationComponent>()->GetCurrentState() != AnimationComponent::States::Spawning)
			{
				GameObjectId id = i;
				aTriggeringNodeInstance->WriteData(3, id);
				aTriggeringNodeInstance->ExitVia(2);
			}
		}
		else
		{
			i->GetComponent<AIController>()->SetState(AIController::AIStates::Idle);
		}
	}*/

	return 1;
}
