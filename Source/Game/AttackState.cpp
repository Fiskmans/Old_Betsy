#include "pch.h"
#include "AttackState.h"
#include "Entity.h"
#include "AIController.h"
#include "CharacterInstance.h"
#include "Life.h"
#include "AnimationComponent.h"
#include "AIPollingStation.h"
#include "GBPhysXKinematicComponent.h"

AttackState::AttackState(Entity* aEntity, AIPollingStation* aPollingStation) :
	myEntity(aEntity),
	myPollingStation(aPollingStation)
{
	myAttackCooldown = 2;
	myAttackCooldownTimer = 0;
}

AttackState::~AttackState()
{
}

bool AttackState::CheckCondition(AIStates& aState)
{
	V3F enemyDir = myEntity->GetPosition() - myPollingStation->GetPlayer()->GetPosition();
	float distanceSqr = enemyDir.LengthSqr();

	if (distanceSqr > 30000.f)
	{
		aState = AIStates::SeekTarget;
		return true;
	}

	aState = AIStates::None;
	return false;
}

void AttackState::Update(float aDeltaTime)
{
	myAttackCooldownTimer += aDeltaTime;

	if (myAttackCooldownTimer > myAttackCooldown)
	{
		myAttackCooldownTimer = 0;
		//std::cout << "Attacking" << std::endl;

		//myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::AttackMelee);

		if (V3F(myPollingStation->GetPlayer()->GetPosition() - myEntity->GetPosition()).LengthSqr() < 30000.f)
		{
			//myPollingStation->GetPlayer()->GetComponent<Life>()->TakeDamage(myAttackDamage);
		}
	}

	return;
}

void AttackState::OnEnter()
{
	std::cout << "Attack state" << std::endl;
}

void AttackState::OnExit()
{
}