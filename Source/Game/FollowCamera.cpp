#include "pch.h"
#include "FollowCamera.h"
#include "Entity.h"
#include "Scene.h"
#include "Camera.h"

#include "DebugDrawer.h"
#include "perlin_noise.h"


FollowCamera::FollowCamera() :
	myCamera(nullptr),
	myEntity(nullptr),
	myTargetEntity(nullptr)
{

}

FollowCamera::~FollowCamera()
{
	UnSubscribeToMessage(MessageType::StartCameraShake);
	UnSubscribeToMessage(MessageType::SetCameraOffset);
}

void FollowCamera::Init(Entity* aEntity)
{
	myEntity = aEntity;
	SubscribeToMessage(MessageType::StartCameraShake);
	SubscribeToMessage(MessageType::SetCameraOffset);

	myDefaultCameraOffset = { 0, 700, -900 };
	myTargetOffset = myDefaultCameraOffset;
	myCurrentOffset = myDefaultCameraOffset;
}

void FollowCamera::SetTargetEntity(Entity* aTargetEntity)
{
	myTargetEntity = aTargetEntity;
	
	CommonUtilities::Matrix4x4<float> mat;
	//mat *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(1.0f);
	mat *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(3.1415926f / 4.0f);
	mat *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(3.1415926f / 4.0f);
	myEntity->SetRotation(mat);
}

void FollowCamera::SetTargetCamera(Camera* aCamera)
{
	myCamera = aCamera;
}

void FollowCamera::Update(const float aDeltaTime)
{
	// FOV CHANGE
	if (myCamera)
	{
		if (myIsChangingFov)
		{
			float currentFov = myCamera->GetFoV();
			if (currentFov < myTargetFov)
			{
				myCamera->SetFov(currentFov + aDeltaTime * 3);
				if (myCamera->GetFoV() > myTargetFov)
				{
					myCamera->SetFov(myTargetFov);
					myIsChangingFov = false;
				}
			}
			else
			{
				myCamera->SetFov(currentFov - aDeltaTime);
				if (myCamera->GetFoV() < myTargetFov)
				{
					myCamera->SetFov(myTargetFov);
					myIsChangingFov = false;
				}
			}
		}
	}

	//Position and lookat
	myCurrentOffset = LERP(myCurrentOffset, myTargetOffset, 0.01f);

	V3F targetEntityPos = myTargetEntity->GetPosition();
	V3F currentPos = myEntity->GetPosition();

	V3F targetPos = CommonUtilities::Vector3<float>(targetEntityPos.x + myCurrentOffset.x,
													targetEntityPos.y + myCurrentOffset.y,
													targetEntityPos.z + myCurrentOffset.z);

	V3F newPos = LERP(currentPos, targetPos, 0.025f);
	myEntity->SetPosition(newPos);

	if (myCamera != nullptr)
	{
		myCamera->SetPosition(newPos);
		myCamera->LookAt(newPos - myCurrentOffset);
	}

}

void FollowCamera::Reset()
{
	myCamera = nullptr;
	myEntity = nullptr;
	myTargetEntity = nullptr;
	UnSubscribeToMessage(MessageType::StartCameraShake);
}

void FollowCamera::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::StartCameraShake)
	{
		myCameraShakeCurrentMovementValue = myCameraShakeStartMovementValue * aMessage.myFloatValue;
	}
	else if (aMessage.myMessageType == MessageType::SetCameraOffset)
	{
		myTargetOffset = *reinterpret_cast<V3F*>(aMessage.myData);
	}
}

void FollowCamera::SetCameraTargetFov(int aFov)
{
	myTargetFov = aFov;
	myIsChangingFov = true;
}

void FollowCamera::SetCameraTargetOffset(int aZOffset, int aYOffset)
{

}

Camera* FollowCamera::GetCamera()
{
	return myCamera;
}

void FollowCamera::OnAttach()
{
}

void FollowCamera::OnDetach()
{
}

void FollowCamera::OnKillMe()
{
}

