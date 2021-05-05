#include "pch.h"
#include "Movement3D.h"
#include "Entity.h"
#include "Audio.h"
#include "DebugDrawer.h"
#include <PathFinder.h>

#define MARKERDURATION 0.5f

#ifdef _DEBUG
bool Movement3D::ourShowPaths;
#endif

Movement3D::Movement3D() :
	myAccelerationSpeed(1.0f),
	myCanTurn(true),
	myCurrentDirectionVector(CommonUtilities::Vector3<float>(0.0f, 0.0f, 1.0f)),
	myTurnRate(2.0f),
	myAnimationPauseTimer(-1.0f),
	myTargetMarker(nullptr),
	myRenderMarkerTimer(0.f)
{
	myIsMoving = false;

	myFootStepInterval = 0.0f;
}

Movement3D::~Movement3D()
{
}

void Movement3D::Init(Entity* aParentEntity)
{
	LOGVERBOSE("Movement3D Inited")
	myIsMoving = false;
	myEntity = aParentEntity;

	ClearMovementTargetPoints();
}

void Movement3D::Update(const float aDeltaTime)
{
	if (myAnimationPauseTimer > 0.0f)
	{
		myAnimationPauseTimer -= aDeltaTime;

		if (myAnimationPauseTimer <= 0)
		{
			ClearMovementTargetPoints();
		}
	}

	if (myTargetMarker)
	{
		if (myRenderMarkerTimer > 0)
		{
			myRenderMarkerTimer -= aDeltaTime;
		}
		else
		{
			myTargetMarker->SetShouldRender(false);
		}
	}

	if (myAnimationPauseTimer <= 0.0f)
	{

		if (myMovementTargetPoints.size() > 0)
		{
#ifdef _DEBUG
			if (ourShowPaths)
			{
				V3F at = myEntity->GetPosition();
				DebugDrawer::GetInstance().SetColor(V4F(0.3f, 1.f, 0.3f, myEntity->GetEntityType() == EntityType::Player ? 1.f : 0.2f));
				for (auto& i : myMovementTargetPoints)
				{
					V3F next = i;
					DebugDrawer::GetInstance().DrawArrow(at, next);
					at = next;
				}
			}
#endif // _DEBUG
			if (!myIsMoving)
			{
				myEntity->SendEntityMessage(EntityMessage::StartWalking);
				myIsMoving = true;
			}
			CommonUtilities::Vector3<float> curPos = myEntity->GetPosition();

			if (myEntity->GetEntityType() == EntityType::Player)
			{
				mySpeed = myPlayerSpeed;
			}

			float movementVal = mySpeed * aDeltaTime;


			myMovementTargetPoints.front().y = 0;
			curPos.y = 0;

			float distance = (myMovementTargetPoints.front() - curPos).Length();
			while (distance < movementVal)
			{
				myMovementTargetPoints.erase(myMovementTargetPoints.begin());

				if (myMovementTargetPoints.empty())
				{
					return;
				}
				else
				{
					myCurrentTargetPoint = myMovementTargetPoints.front();
				}
			}

			if (myCurrentTargetPoint != curPos)
			{
				myTargetDirection = myCurrentTargetPoint - curPos;
				myTargetDirection.y = 0;
				myTargetDirection.Normalize();
			}

			myFinnishedRotating = false;

			myEntity->SetPosition(curPos + myTargetDirection * movementVal);

			//Only for audio. should be removed if anim callback is in.

			if (myEntity->GetComponent<Audio>())
			{
				if (myFootStepInterval <= 0)
				{
					myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::Footstep);
					myFootStepInterval = 0.32f;
				}
				else
				{
					myFootStepInterval -= aDeltaTime;
				}
			}
			//----------
		}
		else
		{
			if (myIsMoving)
			{
				myEntity->SendEntityMessage(EntityMessage::StopWalking);
				myIsMoving = false;
			}
		}

		//Rotation
		if (!myFinnishedRotating)
		{
			auto curRot = myEntity->GetRotation();
			CommonUtilities::Vector4<float> forward(0, 0, 1, 0);
			forward = forward * curRot;
			V3F curforward = V3F(forward.x, 0.f, forward.z).GetNormalized();

			V2F tardir2d = V2F(myTargetDirection.x, myTargetDirection.z);
			V2F curdir2d = V2F(curforward.x, curforward.z);

			float anglediffinrad = acosf(tardir2d.Dot(curdir2d));

			float angledifftar = atan2f(tardir2d.y, tardir2d.x);
			float anglediffcur = atan2f(curdir2d.y, curdir2d.x);

			float pie2 = 2 * PI;

			if (anglediffcur < 0)
			{
				anglediffcur += pie2;
			}

			if (anglediffcur > pie2)
			{
				anglediffcur -= pie2;
			}

			if (angledifftar < 0)
			{
				angledifftar += pie2;
			}

			float dif = MIN(abs(angledifftar - anglediffcur), abs(anglediffcur - angledifftar));

			float radstorotate = CLAMP(0,8.8f*aDeltaTime,dif * 8.f * aDeltaTime);
			float radtorotate = radstorotate;
			if (dif > 0.1f)
			{
				if (angledifftar >= anglediffcur)
				{
					if (abs(angledifftar - anglediffcur) < PI)
					{
						radtorotate = -radstorotate;
					}
				}
				else
				{
					if (abs(angledifftar - anglediffcur) > PI)
					{
						radtorotate = -radstorotate;
					}
				}

				M44F newRot = M44F::CreateRotationAroundY(radtorotate);

				myEntity->SetRotation(curRot * newRot);


			}
			else if (!myFinnishedRotating)
			{
				myFinnishedRotating = true;
			}
		}
		//---------------------
	}
}

void Movement3D::Reset()
{
	LOGVERBOSE("Movement3D Reset")
		myIsMoving = false;
}

V3F Movement3D::GetDirection()
{
	return myCurrentDirectionVector;
}

float Movement3D::GetSpeed()
{
	return mySpeed;
}

void Movement3D::SetSpeed(const float& aSpeed)
{
	mySpeed = aSpeed;
}

float Movement3D::GetDistanceLeft()
{
	V3F at = myEntity->GetPosition();
	float length = 0.f;
	for (auto& i : myMovementTargetPoints)
	{
		length += i.Distance(at);
		at = i;
	}
	return length;
}

void Movement3D::SetValues(float aSpeed, CommonUtilities::Vector3<float> aDirectionVector, float aAccelerationSpeed, float aTurnRate)
{
	mySpeed = aSpeed;
	myAccelerationSpeed = aAccelerationSpeed;
	myCurrentDirectionVector = aDirectionVector;
	myTurnRate = aTurnRate;
}

void Movement3D::SetTargetMarker(ModelInstance* aMarker)
{
	myTargetMarker = aMarker;
}

void Movement3D::SetTargetPosition(V3F aPosition)
{
	std::vector<V3F> points = PathFinder::GetInstance().FindPath(myEntity->GetPosition(), aPosition);
	if (!points.empty())
	{
		AddMovementTargetPoints(points, true);

		if (myTargetMarker)
		{
			myRenderMarkerTimer = MARKERDURATION;
			myTargetMarker->SetPosition(V4F(aPosition, 1.f));
			myTargetMarker->SetShouldRender(true);
		}
	}
	//AddMovementTargetPoint(aPosition, true);
}

bool Movement3D::SetTargetPositionOnNavmeshAtPosition(V3F aPosition)
{
	V3F setPos;

	V3F rayStart = aPosition;
	rayStart.y += 100;

	V3F rayDir = aPosition - rayStart;
	rayDir.Normalize();

	SlabRay ray(rayStart, rayDir);
	setPos = PathFinder::GetInstance().FindPoint(ray);

	if (setPos.LengthSqr() > 0)
	{
		SetTargetPosition(setPos);
		return true;
	}
	else
	{
		return false;
		//SetTargetPosition(aPosition);
	}
}

void Movement3D::AddMovementTargetPoint(CommonUtilities::Vector3<float> aTargetPoint, bool aShouldClearList)
{
	if (aShouldClearList)
	{
		myMovementTargetPoints.clear();
	}

	myMovementTargetPoints.push_back(aTargetPoint);

	myCurrentTargetPoint = myMovementTargetPoints[0];

	if (myTargetMarker)
	{
		myRenderMarkerTimer = MARKERDURATION;
		myTargetMarker->SetPosition(V4F(aTargetPoint, 1));
		myTargetMarker->SetShouldRender(true);
	}

	//FaceTowards(myCurrentTargetPoint);
}

void Movement3D::AddMovementTargetPoints(std::vector<CommonUtilities::Vector3<float>> someTargetPoints, bool aShouldClearList)
{
	if (aShouldClearList)
	{
		myMovementTargetPoints.clear();
	}
	if (someTargetPoints.empty())
	{
		return;
	}

	for (auto& it : someTargetPoints)
	{
		if ((it - myEntity->GetPosition()).LengthSqr() > 0)
		{
			myMovementTargetPoints.push_back(it);
		}
	}

	if (myMovementTargetPoints.empty())
	{
		return;
	}

	myCurrentTargetPoint = myMovementTargetPoints[0];

	if (myTargetMarker)
	{
		myRenderMarkerTimer = MARKERDURATION;
		myTargetMarker->SetPosition(V4F(myMovementTargetPoints.back(), 1));
		myTargetMarker->SetShouldRender(true);
	}
	//FaceTowards(myCurrentTargetPoint);
}

void Movement3D::ClearMovementTargetPoints()
{
	myMovementTargetPoints.clear();
}

void Movement3D::SetAnimationPauseTimer(float aAnimationPauseDuration)
{
	myAnimationPauseTimer = aAnimationPauseDuration;
}

void Movement3D::FaceTowards(V3F aPoint, bool aBruteForce)
{
	if (myAnimationPauseTimer < 0.0f || aBruteForce)
	{
		myTargetDirection = (aPoint - myEntity->GetPosition()).GetNormalized();
		myTargetDirection.y = 0;
		myEntity->SetRotation(CommonUtilities::Matrix4x4<float>::CreateRotationFromDirection(myTargetDirection));
		myCurrentDirectionVector = myTargetDirection;
	}
}

void Movement3D::SetTargetDirectionToPoint(V3F aPoint)
{
	myTargetDirection = (aPoint - myEntity->GetPosition()).GetNormalized();
	myTargetDirection.y = 0;
	myFinnishedRotating = false;
}

bool Movement3D::empty()
{
	return myMovementTargetPoints.empty();
}

bool Movement3D::IsMoving()
{
	return myIsMoving;
}

void Movement3D::OnAttach()
{
}

void Movement3D::OnDetach()
{
}

void Movement3D::OnKillMe()
{
}
