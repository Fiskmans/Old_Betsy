#pragma once
#include "AIBaseState.h"

class AIPollingStation;

class AlertedState : public AIBaseState
{
public:
	AlertedState(Entity* aParentEntity, AIPollingStation* aPollingStation);
	~AlertedState();

	bool CheckCondition(AIStates& aState) override;

	void Update(float aDeltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	void SetAlertAnimFinnished();

private:

	bool myIsPlayingAlertedAnim;
	float mySafetyTimer;

	Entity* myEntity;
	AIPollingStation* myPollingStation;
};
