#pragma once
#include "Component.h"

class GBPhysX;
class GBPhysXCharacter;
class Mesh;

enum class MovementState
{
	Walk,
	Run,
	Crouch
};

class GBPhysXKinematicComponent : public Component
{
public:
	GBPhysXKinematicComponent();
	~GBPhysXKinematicComponent();

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void AddGBPhysXCharacter(GBPhysX* aGBPhysX, const V3F& aPosition, const V3F& aRotation, float aHeight, float aRadius, bool aIsPlayer);
	void SetTargetDirection(const V3F& aDirection);
	void StopMoving();
	void Jump();
	void Run();
	void Walk();
	void Crouch();
	void Teleport(const V3F& aPosition);
	V3F GetDeltaMovement();
	GBPhysXCharacter* GetPhysXCharacter();
	void SetUpdateMovement(bool aShouldUpdate);

protected:

private:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

	GBPhysXCharacter* myGBPhysXCharacter;
	Mesh* myMeshPtr;

	V3F myDirection;
	V3F myMeshDirection;
	float myCurrentSpeed;

	V3F myCurrentVelocity;
	V3F myTargetVelocity;
	float myAccModifyer;

	V3F myDeltaMovement;
	V3F myLastPosition;

	float myCurrentFallVelocity;

	float mySpeedModifyer;

	MovementState myCurrentMovementState;
	float myCurrentSpeedModifyerMode;
	float myCurrentAccModifyerMode;

	int myFrameCounter;

	float myRunSpeedModifyer = 1.8f;
	float myCrouchSpeedModifyer = 0.5f;
	float myWalkSpeedModifyer = 1.0f;
	float myPlayerBaseSpeed = 400.f;
	//float myEnemyBaseSpeed = 300.f;

	bool myUpdateMovement;

	//GAMEMETRIC(float, myRunSpeedModifyer, PLAYER_RUNSPEEDMODIFYER, 1.8f);
	//GAMEMETRIC(float, myCrouchSpeedModifyer, PLAYER_CROUCHSPEEDMODIFYER, 0.5f);
	//GAMEMETRIC(float, myWalkSpeedModifyer, PLAYER_WALKSPEEDMODIFYER, 1.0f);
	//GAMEMETRIC(float, myPlayerBaseSpeed, PLAYER_BASESPEED, 400.0f);
	GAMEMETRIC(float, myEnemyBaseSpeed, ANIMAL_SPEED, 200.0f);
};
