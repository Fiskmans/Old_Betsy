#ifndef ENGINE_GRAPHICS_RENDER_SCENE_H
#define ENGINE_GRAPHICS_RENDER_SCENE_H

#include "engine/graphics/Camera.h"
#include "engine/graphics/Environmentlight.h"

#include "common/Macros.h"

#include "engine/assets/ModelInstance.h"
#include "engine/assets/PointLight.h"

#include "tools/PlaneVolume.h"
#include "tools/Sphere.h"

#include <vector>

namespace engine
{

	class RenderScene
	{
	public:
		void ImGui();

		void AddToScene(ModelInstance* aModel);
		void AddToScene(PointLight* aLight);

		void SetMainCamera(Camera* aCamera) { myMainCamera = aCamera; }
		void SetEnvironmentLight(engine::graphics::EnvironmentLight* aLight) { myEnvironmentLight = aLight; };

		void RemoveFromScene(ModelInstance* aModel);
		void RemoveFromScene(PointLight* aLight);

		void SetSkybox(ModelInstance* aSkybox) { mySkybox = aSkybox;  }


		inline ModelInstance* GetSkybox() { return mySkybox; }
		inline Camera* GetMainCamera() { return myMainCamera; }
		engine::graphics::EnvironmentLight* GetEnvironmentLight() const { return myEnvironmentLight; }

		std::vector<ModelInstance*> CullByFrustum(const tools::Frustum<float>& aPlaneVolume);
		std::vector<ModelInstance*> Cull(const tools::Sphere<float>& aBoundingSphere);

		std::array<PointLight*, NUMBEROFPOINTLIGHTS> CullPointLights(ModelInstance* aModel);

		inline std::vector<ModelInstance*>::iterator begin() { return myModels.begin(); }
		inline std::vector<ModelInstance*>::iterator end() { return myModels.end(); }

	private:
		std::vector<ModelInstance*> myModels;
		std::vector<PointLight*> myPointLights;
		Camera* myMainCamera = nullptr;
		ModelInstance* mySkybox = nullptr;
		engine::graphics::EnvironmentLight* myEnvironmentLight = nullptr;
	};

}

#endif