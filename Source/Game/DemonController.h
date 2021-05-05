#pragma once
#include "Component.h"
#include "Publisher.hpp"

class AnimationComponent;
class Life;
class Movement3D;

class DemonController :
	public Component, public Publisher
{
public:
	enum class DemonState
	{
		Idle,
		MeleeSingle1Attack,
		MeleeSingle2Attack,
		MeleeDoubleAttack,
		RangedAttack,
		BeamAttack,
		Death
	};

	DemonController();
	~DemonController();

	void Init(Entity* aEntity) override;

	void SetTarget(Entity* aEntity);
	void Update(const float aDeltaTime) override;
	void Reset() override;

	Entity* GetTarget();

	virtual void Collided();
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void SetState(DemonState aState);

	bool withinRangeOfPlayer = false;

	void SetSavedVariables();

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	bool myIsDead = false;
	DemonState myCurrentState;
	Entity* myTarget;

	float myUpdatePathfindingTimer;

	float myIdleTimer;
	int myComboCounter;

	float myMeleeSingleAttack1AnimationTimer;
	float myMeleeSingleAttack2AnimationTimer;
	float myMeleeDoubleAttackAnimationTimer;

	float myRangedAttackAnimationTimer;
	float myRangedCastTimer;

	float myBeamAttackAnimationTimer;
	float myBeamStartTargetRotation;
	float myBeamEndTargetRotation;
	bool myBeamSpawnReady;

	float myDeathAnimationTimer;

	float mySavedCurrentRotation;
	float mySavedTargetRotation;
	float mySavedDefaultRotation;

	CharacterInstance* myCharacterInstance;
	Life* myLifeComponent;
	AnimationComponent* myAnimationComponent;
	Movement3D* myMovement3D;

	V3F myCurrentPos;

	std::vector<CommonUtilities::Vector3<float>> myPossibleAoeLocations;
};