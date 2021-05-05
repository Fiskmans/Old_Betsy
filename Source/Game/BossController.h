#pragma once
#include "Component.h"
#include "Publisher.hpp"

class AnimationComponent;
class Life;
class Movement3D;

class BossController :
	public Component, public Publisher
{
public:
	enum class BossState
	{
		Idle,
		HuntTarget,
		RangedAttack,
		MeleeAttack,
		TeleportStart,
		TeleportWait,
		TeleportEmerge,
		SummonMinnions,
		Death
	};

	BossController();
	~BossController();

	void Init(Entity* aEntity) override;

	void SetTarget(Entity* aEntity);
	void Update(const float aDeltaTime) override;
	void Reset() override;

	Entity* GetTarget();

	virtual void Collided();
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void SetState(BossState aState);

	bool withinRangeOfPlayer = false;

	void SetSavedVariables();

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	bool myIsDead = false;
	BossState myCurrentState;
	Entity* myTarget;

	float myUpdatePathfindingTimer;

	float myIdleTimer;

	float myTeleportTimer;
	float myRangedAttackTimer;
	float myMeleeAttackTimer;
	float mySummonTimer;
	float myAnimationTimer;

	float myTeleportDiveTimer;
	float myTeleportWaitTimer;
	float myTeleportEmergeTimer;

	float myRangedAttackAnimationTimer;
	float myMeleeAttackAnimationTimer;
	float mySummonAnimationTimer;

	float myPhase2Duration;
	float myTimerVisualDamageTimer;

	float myRangedAttackRangeSqrd;
	float myMeleeAttackRangeSqrd;
	CharacterInstance* myCharacterInstance;

	Life* myLifeComponent;
	AnimationComponent* myAnimationComponent;
	Movement3D* myMovement3D;
	Collision* myCollisionComponent;

	V3F myCurrentPos;
	int myPhase;

	std::array<CommonUtilities::Vector3<float>, 10> myPossibleTeleportLocations;

	int myCurrentTeleportTarget;
	bool testBool;
};

