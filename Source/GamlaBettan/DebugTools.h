#pragma once
#ifdef _DEBUG
#include "ModelLoader.h"
#include <PlaneVolume.hpp>
#include <map>
#include <vector>
#include <string>
#include <set>
#include "../Game/ImGuiPackage.h"

class LightLoader;
class SlabRay;
class ModelInstance;
class Scene;
class DebugTools
{
public:
	static class ModelLoader* myModelLoader;
	static CommonUtilities::PlaneVolume<float> CameraFrustum;
	static std::map<std::string, std::vector<std::string>>* FileList;
	static std::string myUsername;
	static LightLoader* myLightLoader;
	static SlabRay* LastKnownMouseRay;
	static Camera* myCamera;
	static std::set<std::wstring> CommandLineFlags;

	static void AttachToGizmo(V3F& aVector);
	static bool IsGizmo(V3F& aVector);

	static void UpdateGizmo();
	static void Setup(Scene* aScene);


	static Scene* myScene;
private:

	static ModelInstance* gizmoParts[6];
	static V3F* myGizmoVector;
};
#endif // DEBUG