#pragma once
#ifdef _DEBUG

#include "CommonUtilities\PlaneVolume.hpp"
#include "CommonUtilities\Ray.hpp"

class LightLoader;
class ModelInstance;
class Scene;
class Camera;

class DebugTools
{
public:
	static FRay* LastKnownMouseRay;
	static Camera* myCamera;

	static void AttachToGizmo(V3F& aVector);
	static bool IsGizmo(V3F& aVector);

	static void UpdateGizmo();
	static void Setup();

private:

	static bool myGizmoInScene;
	static ModelInstance* gizmoParts[6];
	static V3F* myGizmoVector;
};
#endif // DEBUG
