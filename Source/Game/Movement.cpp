#include "pch.pch"
#include "Movement.h"
#include "Entity.h"

Movement::Movement() : 
	myAccelerationSpeed(1.0f),
	myCanRotate(true),
	myCanStrafe(true),
	myCanTurn(true),
	myCurrentDirectionVector(CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.1f)),
	myDeaccelerationSpeed(1.0f),
	myRotationSpeed(1.0f),
	mySpeed(0.0f),
	myStrafeSpeed(0.0f),
	myTurnRate(0.0f)
{
	myComponentType = ComponentType::Movement;
}

Movement::~Movement()
{
}

void Movement::Init(Entity* aParentEntity)
{
	myEntity = aParentEntity;
	// SET my variables to some loaddata
	mySpeed = 1.0f;
}

void Movement::Update(const float aDeltaTime)
{
	myEntity->SetPosition(myEntity->GetPosition() + myCurrentDirectionVector);
}

void Movement::Reset()
{
}
