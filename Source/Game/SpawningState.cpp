#include "pch.h"
#include "SpawningState.h"
#include "Entity.h"
#include <iostream>
#include "AnimationComponent.h"
#include "AIPollingStation.h"
#include "AIController.h"
#include "Audio.h"

SpawningState::SpawningState(Entity* aParentEntity, AIPollingStation* aPollingStation) :
	myEntity(aParentEntity),
	myPollingStation(aPollingStation)
{
	mySpawnAnimationOver = false;
}

SpawningState::~SpawningState()
{
}

bool SpawningState::CheckCondition(AIStates& aState)
{
	aState = AIStates::None;

	if (mySpawnAnimationOver)
	{
		aState = AIStates::Idle;
		return true;
	}

	return false;
}

void SpawningState::Update(float aDeltaTime)
{
}

void SpawningState::OnEnter()
{
	mySpawnAnimationOver = false;

	//myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Spawning);
}

void SpawningState::OnExit()
{
}

void SpawningState::SetSpawnAnimationFinnished()
{
	mySpawnAnimationOver = true;
}
