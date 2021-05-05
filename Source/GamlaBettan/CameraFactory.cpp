#include "pch.h"
#include "CameraFactory.h"

#include "Camera.h"
#include "WindowHandler.h"

WindowHandler* CCameraFactory::ourWindowHandler = nullptr;
std::vector<Camera*> CCameraFactory::ourCameras;

float CCameraFactory::ourNear;
float CCameraFactory::ourFar;


bool CCameraFactory::Init(WindowHandler* aWindowHandler, float aNear, float aFar)
{
	if (!aWindowHandler)
	{
		SYSERROR("Camerafactory was passed a nonexisting windowhandler","");
		return false;
	}
	ourWindowHandler = aWindowHandler;
	ourFar = aFar;
	ourNear = aNear;
	return true;
}

Camera* CCameraFactory::CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes)
{
	if (!ourWindowHandler)
	{
		SYSERROR("Tried to create a camera in a not started factory","");
		return nullptr;
	}
	Camera* camera = new Camera();
	float width = ourWindowHandler->GetWidth();
	float height = ourWindowHandler->GetHeight();
	camera->Init(aFoV, CommonUtilities::Vector2<float>(width, height), ourNear, ourFar, aShouldAdaptToWindowRes);
	ourCameras.push_back(camera);

	return camera;
}

Camera* CCameraFactory::CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes, float aCustomNear, float aCustomFar)
{
	if (!ourWindowHandler)
	{
		SYSERROR("Tried to create a camera in a not started factory","");
		return nullptr;
	}
	Camera* camera = new Camera();
	float width = ourWindowHandler->GetWidth();
	float height = ourWindowHandler->GetHeight();
	camera->Init(aFoV, CommonUtilities::Vector2<float>(width, height), aCustomNear, aCustomFar, aShouldAdaptToWindowRes);
	ourCameras.push_back(camera);

	return camera;
}

Camera* CCameraFactory::CreateCamera(Camera::CameraType aType)
{
	if (!ourWindowHandler)
	{
		SYSERROR("Tried to create a camera in a not started factory", "");
		return nullptr;
	}
	Camera* camera = new Camera();
	float width = ourWindowHandler->GetWidth();
	float height = ourWindowHandler->GetHeight();
	camera->Init(V3F(ENVIRONMENCAMEREASIZE, ENVIRONMENCAMEREASIZE, ENVIRONMENCAMEREASIZE));
	ourCameras.push_back(camera);

	return camera;
}
