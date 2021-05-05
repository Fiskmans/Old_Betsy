#pragma once
#include "Component.h"
#include "Vector.hpp"

class Movement : public Component
{
public:

	Movement();
	~Movement();

	virtual void Init(Entity* aParentEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

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

