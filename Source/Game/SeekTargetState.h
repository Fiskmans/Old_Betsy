#pragma once
#include "AIBaseState.h"
#include "Vector3.hpp"

class AIPollingStation;

class SeekTargetState : public AIBaseState
{
public:
	SeekTargetState(Entity* aEntity, AIPollingStation* aPollingStation);
	~SeekTargetState();

	bool CheckCondition(AIStates& aState) override;

	bool UpdateSeparation(V3F aDirection);
	void UpdatePathfinder();
	void UpdateMovement();

	void Update(float aDeltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	void SetStunlocked(bool aIsStunned);

private:
	Entity* myEntity;
	AIPollingStation* myPollingStation;

	bool myIsStunlocked;

	std::vector<V3F> myWayPoints;

	V3F myTargetPos;

	float myUpdatePathfindingTimer;
};
