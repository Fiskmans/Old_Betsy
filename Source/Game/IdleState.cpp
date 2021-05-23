#include "pch.h"
#include "IdleState.h"
#include "Entity.h"
#include <iostream>
#include "AnimationComponent.h"
#include "AIPollingStation.h"
#include "AIController.h"
#include "Audio.h"

IdleState::IdleState(Entity* aParentEntity, AIPollingStation* aPollingStation):
	myEntity(aParentEntity),
	myPollingStation(aPollingStation)
{

}

IdleState::~IdleState()
{
}

bool IdleState::CheckCondition(AIStates& aState)
{
	aState = AIStates::None;

	float aggroDistance = 500;
	float idleCloseDistance = 30000;

	float lengthSqr = V3F(myPollingStation->GetPlayer()->GetPosition() - myEntity->GetPosition()).LengthSqr();

	if (lengthSqr < (aggroDistance * aggroDistance) && lengthSqr > idleCloseDistance)
	{
		aState = AIStates::SeekTarget;
		return true;
	}
	else if (myEntity->GetComponent<AIController>()->GetTargetPosition() != V3F() &&
		myEntity->GetPosition().DistanceSqr(myEntity->GetComponent<AIController>()->GetTargetPosition()) > 500 * 500)
	{
		aState = AIStates::SeekTarget;
		return true;
	}
	else if (myEntity->GetComponent<AIController>()->GetTargetEntity() != nullptr)
	{
		aState = AIStates::SeekTarget;
		return true;
	}


	return false;
}

void IdleState::Update(float aDeltaTime)
{
	//start ideling whatever that is
}

void IdleState::OnEnter()
{
	myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
}

void IdleState::OnExit()
{
}
