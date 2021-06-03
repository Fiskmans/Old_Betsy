#pragma once
#include "Component.h"

#include "Publisher.hpp"

class AIBaseState;
class AIPollingStation;
enum class AIStates;
class ComponentLake;

class AIController : public Component, public Publisher, public Observer
{
public:

	AIController();
	~AIController();

	void Init(Entity* aEntity) override;

	void AIInit(AIPollingStation* aPollingStation, AIStates aStartingState, int aCharacterID, bool aSkipAlerted = false);
	AIPollingStation* GetPollingStation();
	
	void Update(const float aDeltaTime) override;
	void Reset() override;

	virtual void Collided();
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void RecieveMessage(const Message& aMessage) override;

	void SetTargetPosition(const V3F& aTargetPosition);
	V3F GetTargetPosition();

	void SetTargetEntity(Entity* aEntity);
	Entity* GetTargetEntity();
	int GetCharacterID();

	void SwitchState(AIStates aState);

	void OnDisable() override;

	bool withinRangeOfPlayer = false;
	bool myTookDamage;

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	bool myIsDead = false;
	std::unordered_map<AIStates, AIBaseState*> myStates;
	AIStates myCurrentState;
	AIPollingStation* myPollingStation;
	//AbilityData* myAbilityData;
	int myCharacterID;

	bool myIsAnimLocked;

	V3F myTargetPosition;
	Entity* myTargetEntity;

	float animalSoundTimer = 0;

	float myUpdatePathfindingTimer;
};