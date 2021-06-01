#include "pch.h"
#include "GBPhysXKinematicComponent.h"
#include "AnimationComponent.h"
#include "GBPhysX.h"
#include "DebugDrawer.h"
#ifdef _DEBUG
#include "Entity.h"
#endif // _DEBUG


#define AccWalkModifyer 25.0f
#define AccRunModifyer 10.0f
#define AccCrouchModifyer 25.0f
#define DeAccModifyer 25.0f

GBPhysXKinematicComponent::GBPhysXKinematicComponent() :
	myGBPhysXCharacter(nullptr),
	myMeshPtr(nullptr),
	myCurrentFallVelocity(0.f),
	myFrameCounter(0),
	myUpdateMovement(false)
{
	myCurrentMovementState = MovementState::Walk;
	myCurrentVelocity = V3F();
	myTargetVelocity = V3F();
	myAccModifyer = 0;
	myCurrentAccModifyerMode = 0;
	myCurrentAccModifyerMode = 0;
}

GBPhysXKinematicComponent::~GBPhysXKinematicComponent()
{
	if (myGBPhysXCharacter != nullptr)
	{
		myGBPhysXCharacter->ReleaseHitBoxes();
		myGBPhysXCharacter->RemoveFromScene();
		myGBPhysXCharacter->Release();
		SAFE_DELETE(myGBPhysXCharacter);
	}

	WIPE(*this);
}

void GBPhysXKinematicComponent::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myCurrentFallVelocity = -9.82f;
	myDirection = V3F();
	myDeltaMovement = V3F();
	myLastPosition = V3F();
	mySpeedModifyer = 1.0f;
	myLastPosition = myEntity->GetPosition();
	myMeshDirection = V3F();

	myCurrentSpeed = 0;

	Walk();
}


void GBPhysXKinematicComponent::Update(const float aDeltaTime)
{
	if (myMeshPtr != nullptr)
	{
		if (!myEntity->GetIsAlive())
		{
			myGBPhysXCharacter->Kill();
			return;
		}

		{
			PERFORMANCETAG("movement")
				float baseSpeed;
			if (myEntity->GetEntityType() == EntityType::Player)
			{
				baseSpeed = myPlayerBaseSpeed;
			}
			else
			{
				baseSpeed = myEnemyBaseSpeed;
			}


			V3F lerpDistance = LERP(myCurrentVelocity, myDirection * static_cast<float>(baseSpeed) * mySpeedModifyer, 0.5f);
			myCurrentVelocity += (lerpDistance - myCurrentVelocity) * aDeltaTime * myAccModifyer;

			myCurrentVelocity.y = 0;
			myCurrentVelocity.x = CLAMP(-myPlayerBaseSpeed*myRunSpeedModifyer, myPlayerBaseSpeed*myRunSpeedModifyer, myCurrentVelocity.x); // Lag spike mitigation
			myCurrentVelocity.z = CLAMP(-myPlayerBaseSpeed*myRunSpeedModifyer, myPlayerBaseSpeed*myRunSpeedModifyer, myCurrentVelocity.z);

			if (myUpdateMovement)
			{
				myGBPhysXCharacter->Move(myDirection * baseSpeed * mySpeedModifyer * aDeltaTime, aDeltaTime);
			}

			myDeltaMovement = myLastPosition - myEntity->GetPosition();
			myLastPosition = myEntity->GetPosition();
		}

		{
			PERFORMANCETAG("rotation")
			if (myEntity->GetEntityType() == EntityType::Enemy)
			{
				V3F offset = V3F(0.0f, 100.0f, 0.0f);
				myEntity->SetPosition(myGBPhysXCharacter->GetPosition() - offset);

				//myMeshDirection = LERP(myMeshDirection, myDirection, 0.5f);

				if (myDirection != V3F())
				{
					V3F dir = myDirection;
					dir.y = 0;
					/*if (USEDEBUGLINES)
					{
						DebugDrawer::GetInstance().DrawArrow(myEntity->GetPosition() + V3F(0, 120, 0), myEntity->GetPosition() + V3F(0, 120, 0) + (dir.GetNormalized() * 100.f));
					}*/
					myMeshDirection = LERP(myMeshDirection, myDirection, 0.1f);
					myMeshDirection.y = 0;
					myEntity->SetRotation(CommonUtilities::Matrix4x4<float>::CreateRotationFromDirection(myMeshDirection));
				}
			}
			else
			{
				myEntity->SetPosition(myGBPhysXCharacter->GetPosition());
			}
		}

		{
			PERFORMANCETAG("update hitboxes")
			if ((myEntity->GetEntityID() + myFrameCounter) % 4 == 0)
			{
				//myGBPhysXCharacter->UpdateHitBoxes();
			}
			myFrameCounter++;
			if (myFrameCounter > 3)
			{
				myFrameCounter = 0;
			}
		}
	}
}

void GBPhysXKinematicComponent::Reset()
{
	myMeshPtr = nullptr;
	myGBPhysXCharacter->ReleaseHitBoxes();
	myGBPhysXCharacter->RemoveFromScene();
	myGBPhysXCharacter->Release();
	SAFE_DELETE(myGBPhysXCharacter);

	myEntity = nullptr;
	myCurrentFallVelocity = 0.0f;
}

void GBPhysXKinematicComponent::AddGBPhysXCharacter(GBPhysX* aGBPhysX, const V3F& aPosition, const V3F& aRotation, float aHeight, float aRadius, bool aIsPlayer)
{
	if (myGBPhysXCharacter != nullptr)
	{
		myGBPhysXCharacter->RemoveFromScene();
		myGBPhysXCharacter->Release();
		SAFE_DELETE(myGBPhysXCharacter);
		SYSERROR("ADD GB PHYSXCHARACTER CALLED MORE THAN ONCE ON GB PHYSXKINEMATIC CONTROLLER");
	}

	myGBPhysXCharacter = aGBPhysX->GBCreateCapsuleController(aPosition, aRotation, aHeight, aRadius, aIsPlayer);
	myMeshPtr = myEntity->GetComponent<Mesh>();
	myGBPhysXCharacter->SetIsKinematic(true);
	myGBPhysXCharacter->SetEntity(myEntity);
}

void GBPhysXKinematicComponent::SetTargetDirection(const V3F& aDirection)
{
	myDirection = aDirection;
	mySpeedModifyer = myCurrentSpeedModifyerMode;
	myAccModifyer = myCurrentAccModifyerMode;
}

void GBPhysXKinematicComponent::StopMoving()
{
	myAccModifyer = DeAccModifyer;
	mySpeedModifyer = 0;
}

void GBPhysXKinematicComponent::Jump()
{
	if (myGBPhysXCharacter->GetIsGrounded())
	{
		myGBPhysXCharacter->SetIsGrounded(false);
		myGBPhysXCharacter->Jump();
	}
}

void GBPhysXKinematicComponent::Run()
{
	if (myCurrentMovementState == MovementState::Crouch)
	{
		myGBPhysXCharacter->Stand();
	}

	myCurrentSpeedModifyerMode = myRunSpeedModifyer;
	myCurrentAccModifyerMode = AccRunModifyer;
	myCurrentMovementState = MovementState::Run;
}

void GBPhysXKinematicComponent::Walk()
{
	if (myCurrentMovementState == MovementState::Crouch)
	{
		myGBPhysXCharacter->Stand();
	}

	myCurrentSpeedModifyerMode = myWalkSpeedModifyer;
	myCurrentAccModifyerMode = AccWalkModifyer;
	myCurrentMovementState = MovementState::Walk;
}

void GBPhysXKinematicComponent::Crouch()
{
	myCurrentSpeedModifyerMode = myCrouchSpeedModifyer;
	myCurrentAccModifyerMode = AccCrouchModifyer;
	myGBPhysXCharacter->Crouch();

	myCurrentMovementState = MovementState::Crouch;
}

void GBPhysXKinematicComponent::Teleport(const V3F& aPosition)
{
	myGBPhysXCharacter->Teleport(aPosition);
}

V3F GBPhysXKinematicComponent::GetDeltaMovement()
{
	return myDeltaMovement;
}

GBPhysXCharacter* GBPhysXKinematicComponent::GetPhysXCharacter()
{
	return myGBPhysXCharacter;
}

void GBPhysXKinematicComponent::SetUpdateMovement(bool aShouldUpdate)
{
	myUpdateMovement = aShouldUpdate;
}

void GBPhysXKinematicComponent::OnAttach()
{
}

void GBPhysXKinematicComponent::OnDetach()
{
}

void GBPhysXKinematicComponent::OnKillMe()
{
}
