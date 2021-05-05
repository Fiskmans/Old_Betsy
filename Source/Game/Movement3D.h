#pragma once
#include "Component.h"
#include "Vector.hpp"
#include "Publisher.hpp"

class ModelInstance;

class Movement3D : public Component, public Publisher
{
public:

	Movement3D();
	~Movement3D();

	virtual void Init(Entity* aParentEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	V3F GetDirection();
	float GetSpeed();
	void SetSpeed(const float& aSpeed);
	float GetDistanceLeft();
	void SetValues(
		float aSpeed,
		CommonUtilities::Vector3<float> aDirectionVector = CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f),
		float aAccelerationSpeed = 1.0f, 
		float aTurnRate = 1.0f);

	void SetTargetMarker(ModelInstance* aMarker);
	void SetTargetPosition(V3F aPosition);
	bool SetTargetPositionOnNavmeshAtPosition(V3F aPosition);
	void AddMovementTargetPoint(CommonUtilities::Vector3<float> aTargetPoint, bool aShouldClearList);
	void AddMovementTargetPoints(std::vector<CommonUtilities::Vector3<float>> someTargetPoints, bool aShouldClearList);
	void ClearMovementTargetPoints();
	void SetAnimationPauseTimer(float aAnimationPauseDuration);
	void FaceTowards(V3F aPoint, bool aBruteForce = false);
	void SetTargetDirectionToPoint(V3F aPoint);
	bool empty();

	bool IsMoving();

#ifdef _DEBUG
	static bool ourShowPaths;
#endif // _DEBUG

protected:       
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:

	bool myIsMoving;
	bool myFinnishedRotating = true;

	float mySpeed;
	float myAccelerationSpeed;

	GAMEMETRIC(float, myPlayerSpeed, PLAYERSPEED, 100.f);

	bool myCanTurn;
	float myTurnRate;

	float myAnimationPauseTimer;

	V3F myCurrentDirectionVector;
	V3F myTargetDirection;

	std::vector<V3F> myMovementTargetPoints;
	V3F myCurrentTargetPoint;

	ModelInstance* myTargetMarker;
	float myRenderMarkerTimer;

	//Ta bort ifall vi får footstep event från animationer
	float myFootStepInterval;
};