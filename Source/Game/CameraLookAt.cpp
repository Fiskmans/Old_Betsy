#include "pch.h"
#include "CameraLookAt.h"
#include <Camera.h>

void CameraLookAt::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void CameraLookAt::Update(const float aDeltaTime)
{
	if (myCamera)
	{
		myCamera->LookAt(myEntity);
	}
}

void CameraLookAt::Reset()
{
}

void CameraLookAt::SetCamera(Camera* aCamera)
{
	myCamera = aCamera;
}

void CameraLookAt::OnAttach()
{
}

void CameraLookAt::OnDetach()
{
}

void CameraLookAt::OnKillMe()
{
}
