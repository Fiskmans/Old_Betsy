#pragma once
#include "AIBaseState.h"

class AIPollingStation;

class SpawningState : public AIBaseState
{
public:
	SpawningState(Entity* aParentEntity, AIPollingStation* aPollingStation);
	~SpawningState();

	bool CheckCondition(AIStates& aState) override;

	void Update(float aDeltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	void SetSpawnAnimationFinnished();

private:
	Entity* myEntity;
	AIPollingStation* myPollingStation;

	float mySpawningTimer;
	bool mySpawnAnimationOver;
};
