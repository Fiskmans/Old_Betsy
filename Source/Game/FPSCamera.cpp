#include "pch.h"
#include "FPSCamera.h"
#include "Entity.h"
#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"

#include "perlin_noise.h"


FPSCamera::FPSCamera() :
	myCamera(nullptr),
	myEntity(nullptr)
{
	myTargetFOV = 0;
}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void FPSCamera::SetCamera(Camera* aCamera)
{
	myCamera = aCamera;
}

void FPSCamera::Update(const float aDeltaTime)
{
	if (myCamera)
	{
		myCamera->SetPosition(myEntity->GetPosition() + (myEntity->GetForward() + myOffset));

		if (myIsChangingFOV)
		{
			float currentFov = myCamera->GetFoV();
			if (currentFov < myTargetFOV)
			{
				myCamera->SetFov(currentFov + (aDeltaTime * myZoomSpeed));
				if (myCamera->GetFoV() > myTargetFOV)
				{
					myCamera->SetFov(myTargetFOV);
					myIsChangingFOV = false;
				}
			}
			else
			{
				myCamera->SetFov(currentFov - aDeltaTime * myZoomSpeed);
				if (myCamera->GetFoV() < myTargetFOV)
				{
					myCamera->SetFov(myTargetFOV);
					myIsChangingFOV = false;
				}
			}
		}
	}
}

void FPSCamera::Reset()
{
	myCamera = nullptr;
	myEntity = nullptr;
}

void FPSCamera::RecieveMessage(const Message& aMessage)
{
}

void FPSCamera::ZoomIn()
{
	SetTargetFOV(myZoomSpeed, myZoomInFOV);
}

void FPSCamera::ZoomOut()
{
	SetTargetFOV(myZoomSpeed, myBaseFOV);
}

void FPSCamera::SetTargetFOV(float aSpeed, float aFOV)
{
	myTargetFOV = aFOV;
	myIsChangingFOV = true;
}

void FPSCamera::SetOffset(const V3F& anOffset)
{
	myOffset = anOffset;
}

Camera* FPSCamera::GetCamera()
{
	return myCamera;
}

void FPSCamera::RotateCameraFromMouse(const V2F& aMouseMovement)
{
	if (myCamera)
	{
		myCamera->RotateWorldSpace(V3F(0.f, aMouseMovement.x, 0.f));
	

		V3F forw = myCamera->GetFlatForward();
		myCamera->Rotate(V3F(aMouseMovement.y, 0, 0));
		V3F postForw = myCamera->GetForward();
		float alignment = forw.Dot(postForw);
		bool lookingUp = postForw.Dot(V3F(0, 1, 0)) > 0.f;
		if (alignment < myMaxVerticalAngle)
		{
			float alph = acos(alignment);
			float beth = acos(myMaxVerticalAngle);
			float delth = alph - beth;
			delth *= lookingUp ? -1.f : 1.f;
			myCamera->Rotate(V3F(-delth, 0.f, 0.f));
		}
	}
}

void FPSCamera::OnAttach()
{
}

void FPSCamera::OnDetach()
{
}

void FPSCamera::OnKillMe()
{
}