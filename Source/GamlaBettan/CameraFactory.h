#pragma once
class Camera;
class WindowHandler;


class CCameraFactory
{
public:
	static bool Init(WindowHandler* aWindowHandler, float aNear, float aFar);
	static Camera* CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes);
	static Camera* CreateCamera(float aFoV, const bool aShouldAdaptToWindowRes, float aCustomNear, float aCustomFar);
	static Camera* CreateCamera(Camera::CameraType aType);

private:
	static WindowHandler* ourWindowHandler;
	static std::vector<Camera*> ourCameras;
	static float ourNear;
	static float ourFar;
};

