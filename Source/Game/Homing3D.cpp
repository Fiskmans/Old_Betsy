#include "pch.h"
#include "Homing3D.h"
#include "Entity.h"

#if USEIMGUI
bool Homing3D::ourShowTarget = false;
#include <DebugDrawer.h>
#endif // USEIMGUI


Homing3D::Homing3D() :
	myTarget(nullptr),
	myAccelerationSpeed(1.0f),
	myCanRotate(true),
	myCanTurn(true),
	myCurrentDirectionVector(CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.1f)),
	myDeaccelerationSpeed(1.0f),
	myRotationSpeed(1.0f),
	mySpeed(0.0f),
	myTurnRate(0.0f)
{

	myShouldStartHoming = false;
	myTimerToStartHoming = 0;
}

Homing3D::~Homing3D()
{
	myTarget = nullptr;
}

void Homing3D::Init(Entity* aParentEntity)
{
	myEntity = aParentEntity;
	if (myEntity->GetEntityType() == EntityType::Enemy)
	{
		myShouldStartHoming = true;
	}
	// SET my variables to some loaddata
	mySpeed = 2.0f;
}

void Homing3D::Update(const float aDeltaTime)
{
	if (myTarget)
	{
		if (myShouldStartHoming)
		{
			//float interpolationPercent = 0.93f;
			float interpolationPercent = 0.60f;

			V3F targetDirection = myTarget->GetPosition() - myEntity->GetPosition();
			targetDirection.Normalize();

			V3F interpolatedDirection = (myCurrentDirectionVector * interpolationPercent) + ((1 - interpolationPercent) * targetDirection);

			interpolatedDirection.Normalize();

			myEntity->SetPosition(myEntity->GetPosition() + interpolatedDirection * mySpeed * aDeltaTime);

			myCurrentDirectionVector = interpolatedDirection;

			myEntity->SetRotation(CU::Matrix4x4<float>::CreateRotationFromDirection(myCurrentDirectionVector));

#if USEIMGUI
			if (ourShowTarget)
			{
				DebugDrawer::GetInstance().DrawArrow(myEntity->GetPosition(), myTarget->GetPosition());
			}
#endif // USEIMGUI

		}
		else
		{
			myEntity->SetPosition(myEntity->GetPosition() + myCurrentDirectionVector * mySpeed * aDeltaTime);
		}

		if (!myTarget->GetIsAlive())
		{
			myTarget = nullptr;
		}

	}

	if (myTimerToStartHoming < 0.6f)
	{
		myTimerToStartHoming += aDeltaTime;
#if USEIMGUI
		if (ourShowTarget)
		{
			DebugDrawer::GetInstance().DrawProgress(myEntity->GetPosition() + V3F(0,5,0), 10,myTimerToStartHoming/0.6f);
		}
#endif // USEIMGUI
	}
	else
	{
		myShouldStartHoming = true;
	}
}

void Homing3D::Reset()
{
	myTimerToStartHoming = 0;
	myShouldStartHoming = false;
	myEntity = nullptr;
	myTarget = nullptr;
	mySpeed = 0;

	myAccelerationSpeed = 0;
	myDeaccelerationSpeed = 0;

	myCanRotate = false;
	myRotationSpeed = 0;

	myCanTurn = false;
	myTurnRate = 0;;

	myCurrentDirectionVector = { 0 };
}

void Homing3D::SetValues(Entity* aTarget, float aSpeed, CommonUtilities::Vector3<float> aDirectionVector, float aAccelerationSpeed, float aDeaccelerationSpeed, float aRotationSpeed, float aTurnRate)
{
	myTarget = aTarget;
	mySpeed = aSpeed;
	myAccelerationSpeed = aAccelerationSpeed;
	myCurrentDirectionVector = aDirectionVector;
	myDeaccelerationSpeed = aDeaccelerationSpeed;
	myRotationSpeed = aRotationSpeed;
	myTurnRate = aTurnRate;
}

void Homing3D::OnAttach()
{
}

void Homing3D::OnDetach()
{
}

void Homing3D::OnKillMe()
{
}
