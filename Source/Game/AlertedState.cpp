#include "pch.h"
#include "AlertedState.h"
#include "AnimationComponent.h"
#include "AIPollingStation.h"
#include "AIController.h"
#include "Audio.h"
#include "Game\Entity.h"

AlertedState::AlertedState(Entity* aParentEntity, AIPollingStation* aPollingStation)
{
	myEntity = aParentEntity;
	myPollingStation = aPollingStation;
	mySafetyTimer = 0;
}

AlertedState::~AlertedState()
{
}

bool AlertedState::CheckCondition(AIStates& aState)
{
	aState = AIStates::None;

	if (!myIsPlayingAlertedAnim || mySafetyTimer <= 0)
	{
		float aggroDistance = 2000;

		if (V3F(myPollingStation->GetPlayer()->GetPosition() - myEntity->GetPosition()).LengthSqr() < (aggroDistance * aggroDistance))
		{
			aState = AIStates::SeekTarget;
			return true;
		}
		else if (myEntity->GetComponent<AIController>()->GetTargetPosition() != V3F() && myEntity->GetPosition().DistanceSqr(myEntity->GetComponent<AIController>()->GetTargetPosition()) > 100 * 100)
		{
			aState = AIStates::SeekTarget;
			return true;
		}
		else
		{
			aState = AIStates::Idle;
			return true;
		}
	}

	return false;
}

void AlertedState::Update(float aDeltaTime)
{
	mySafetyTimer -= aDeltaTime;
}

void AlertedState::OnEnter()
{
	//myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Alerted);
	myIsPlayingAlertedAnim = true;

	mySafetyTimer = 10.f;
}

void AlertedState::OnExit()
{
}

void AlertedState::SetAlertAnimFinnished()
{
	myIsPlayingAlertedAnim = false;
}