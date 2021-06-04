#include "pch.h"
#include "CameraFactory.h"

#include "Camera.h"
#include "WindowHandler.h"

std::vector<Camera*> CCameraFactory::ourCameras;

float CCameraFactory::ourNear;
float CCameraFactory::ourFar;


bool CCameraFactory::Init(float aNear, float aFar)
{
	ourFar = aFar;
	ourNear = aNear;
	return true;
}

Camera* CCameraFactory::CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes)
{
	Camera* camera = new Camera();
	camera->Init(aFoV, WindowHandler::GetInstance().GetSize(), ourNear, ourFar, aShouldAdaptToWindowRes);
	ourCameras.push_back(camera);

	return camera;
}

Camera* CCameraFactory::CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes, float aCustomNear, float aCustomFar)
{
	Camera* camera = new Camera();
	camera->Init(aFoV, WindowHandler::GetInstance().GetSize(), aCustomNear, aCustomFar, aShouldAdaptToWindowRes);
	ourCameras.push_back(camera);

	return camera;
}

Camera* CCameraFactory::CreateCamera(Camera::CameraType aType)
{
	Camera* camera = new Camera();
	camera->Init(V3F(ENVIRONMENCAMEREASIZE, ENVIRONMENCAMEREASIZE, ENVIRONMENCAMEREASIZE));
	ourCameras.push_back(camera);

	return camera;
}
