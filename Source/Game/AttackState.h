#pragma once
#include "AIBaseState.h"

class Stats;
class AIPollingStation;

class AttackState : public AIBaseState, public Publisher
{
public:
	AttackState(Entity* aEntity, AIPollingStation* aPollingStation);
	~AttackState();

	bool CheckCondition(AIStates& aState) override;

	void Update(float aDeltaTime) override;
	void OnEnter() override;
	void OnExit() override;

private:
	Entity* myEntity;
	Entity* myTarget;

	AIPollingStation* myPollingStation;

	float myAttackCooldown;
	float myAttackCooldownTimer;

	GAMEMETRIC(float, myAttackDamage, ZOMBIEDAMAGE, 10);
};