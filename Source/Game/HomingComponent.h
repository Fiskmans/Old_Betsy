#pragma once
#include "Component.h"
#include "Vector.hpp"

class Homing3D : public Component
{
public:

	Homing3D();
	~Homing3D();

	virtual void Init(Entity* aParentEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetValues(
		float aSpeed,
		CommonUtilities::Vector3<float> aDirectionVector = CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f),
		float aAccelerationSpeed = 1.0f,
		float aDeaccelerationSpeed = 1.0f,
		float aRotationSpeed = 1.0f,
		float aStrafeSpeed = 1.0f,
		float aTurnRate = 1.0f);

private:


	void Accelerate();
	void Decelerate();
	void Turn();
	void Move();

	float mySpeed;
	float myAccelerationSpeed;
	float myDeaccelerationSpeed;

	bool myCanRotate;
	float myRotationSpeed;

	bool myCanTurn;
	float myTurnRate;

	bool myCanStrafe;
	float myStrafeSpeed;

	CommonUtilities::Vector3<float> myCurrentDirectionVector;
};
