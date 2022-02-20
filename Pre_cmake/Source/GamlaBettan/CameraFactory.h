#pragma once

#include "GamlaBettan\Camera.h"

class WindowHandler;

class CCameraFactory
{
public:
	static bool Init(float aNear, float aFar);
	static Camera* CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes);
	static Camera* CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes, float aCustomNear, float aCustomFar);
	static Camera* CreateCamera();

private:
	static std::vector<Camera*> ourCameras;
	static float ourNear;
	static float ourFar;
};

