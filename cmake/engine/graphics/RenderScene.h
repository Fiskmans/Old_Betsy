#ifndef ENGINE_GRAPHICS_RENDER_SCENE_H
#define ENGINE_GRAPHICS_RENDER_SCENE_H

#include "common/Macros.h"

#include "engine/graphics/Camera.h"
#include "engine/graphics/Environmentlight.h"

#include "engine/assets/ModelInstance.h"
#include "engine/assets/PointLight.h"

#include "engine/ImGuiWindow.h"

#include "tools/PlaneVolume.h"
#include "tools/Sphere.h"

#include <vector>

namespace engine
{
	class RenderScene : public engine::ImGuiWindow
	{
	public:
		RenderScene(const std::string& aName);

		void OnImgui() override;
		inline const char* ImGuiName() override { return myName.c_str(); };

		void AddToScene(ModelInstance* aModel);
		void AddToScene(PointLight* aLight);

		void RemoveFromScene(ModelInstance* aModel);
		void RemoveFromScene(PointLight* aLight);

		void SetMainCamera(Camera* aCamera) { myMainCamera = aCamera; }
		void SetEnvironmentLight(engine::graphics::EnvironmentLight* aLight) { myEnvironmentLight = aLight; };

		inline Camera* GetMainCamera() { return myMainCamera; }
		engine::graphics::EnvironmentLight* GetEnvironmentLight() const { return myEnvironmentLight; }

		std::vector<ModelInstance*> Cull(const tools::Frustum<float>& aPlaneVolume);
		std::vector<ModelInstance*> Cull(const tools::Sphere<float>& aBoundingSphere);

		std::array<PointLight*, NUMBEROFPOINTLIGHTS> FindAffectingPointLights(ModelInstance* aModel);

		inline std::vector<ModelInstance*>::iterator begin() { return myModels.begin(); }
		inline std::vector<ModelInstance*>::iterator end() { return myModels.end(); }

	private:
		std::string myName;

		std::vector<ModelInstance*> myModels;
		std::vector<PointLight*> myPointLights;
		Camera* myMainCamera = nullptr;
		ModelInstance* mySkybox = nullptr;
		engine::graphics::EnvironmentLight* myEnvironmentLight = nullptr;
	};

}

#endif