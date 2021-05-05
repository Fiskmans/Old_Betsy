#pragma once
#include "AIBaseState.h"

class AIPollingStation;

class IdleState : public AIBaseState
{
public:
	IdleState(Entity* aParentEntity, AIPollingStation* aPollingStation);
	~IdleState();

	bool CheckCondition(AIStates& aState) override;

	void Update(float aDeltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	bool myShouldSkipAlerted = false;

private:
	Entity* myEntity;
	AIPollingStation* myPollingStation;
};