#pragma once
#include "Publisher.hpp"
#include "Observer.hpp"
#include "Component.h"
#include "AIPollingStation.h"
#include "Vector3.hpp"

enum class TraderState
{
	Enter,
	Idle,
	Hello,
	Trading,
	Goodbye,
	Leaving,
	Despawn,
	None
};

class TraderAI : public Component, public Publisher, public Observer
{
public:

	TraderAI();
	~TraderAI();

	void Init(Entity* aEntity) override;

	void AIInit(AIPollingStation* aPollingStation, TraderState aStartingState);
	AIPollingStation* GetPollingStation();

	void Update(const float aDeltaTime) override;
	void Reset() override;

	virtual void Collided();
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void RecieveMessage(const Message& aMessage) override;

	void SetDespawnPosition(const V3F& aDespawnPosition);
	V3F GetDespawnPosition();

	void SetTargetPosition(const V3F& aTargetPosition);
	V3F GetTargetPosition();

	void SetTargetEntity(Entity* aEntity);
	Entity* GetTargetEntity();

	void SwitchState(TraderState aState);

	void StartTrading();
	void StopTrading();

	void OnDisable() override;

	bool withinRangeOfPlayer = false;

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	TraderState myCurrentState;
	TraderState myStateBeforeTrade;
	TraderState myStateBerforeGoodbye;

	AIPollingStation* myPollingStation;

	bool myIsAnimLocked;

	V3F myTargetPosition;
	V3F myDespawnPosition;
	Entity* myTargetEntity;

	float myUpdatePathfindingTimer;

	float myDistanceToPlayerOrTargetpointBeforeSettingUpShop;
	float myTimeToIdleBeforeLeaving;
	float myDistanceToSpawnBeforeDespawn;

	std::vector<V3F> myMovementWayPoints;
};