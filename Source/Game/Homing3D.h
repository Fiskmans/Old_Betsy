#pragma once
#include "Component.h"

class Homing3D : public Component
{
public:

	Homing3D();
	~Homing3D();

	virtual void Init(Entity* aParentEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetValues(
		Entity* aTarget,
		float aSpeed,
		CommonUtilities::Vector3<float> aDirectionVector = CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f),
		float aAccelerationSpeed = 1.0f,
		float aDeaccelerationSpeed = 1.0f,
		float aRotationSpeed = 1.0f,
		float aTurnRate = 1.0f);

#if USEIMGUI
	static bool ourShowTarget;
#endif
protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:

	void Accelerate();
	void Decelerate();
	void Turn();
	void Move();

	Entity* myTarget;

	bool myShouldStartHoming;
	float myTimerToStartHoming;

	float mySpeed;
	float myAccelerationSpeed;
	float myDeaccelerationSpeed;

	bool myCanRotate;
	float myRotationSpeed;

	bool myCanTurn;
	float myTurnRate;

	CommonUtilities::Vector3<float> myCurrentDirectionVector;
};
