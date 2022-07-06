#ifndef ENGINE_GRAPHICS_RENDER_SCENE_H
#define ENGINE_GRAPHICS_RENDER_SCENE_H

#include "engine/graphics/Camera.h"

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
		void Update(float aDeltaTime);

		void AddToScene(ModelInstance* aModel);
		void AddToScene(PointLight* aLight);
		//void AddToScene(SpotLight* aSpotLight);
		//void AddToScene(Decal* aDecal);
		//void AddToScene(SpriteInstance* aSprite);
		//void AddToScene(TextInstance* aText);
		//void AddToScene(ParticleInstance* aParticle);

		void SetMainCamera(Camera* aCamera) { myMainCamera = aCamera; }
		//void SetEnvironmentLight(EnvironmentLight* aLight);

		void RemoveFromScene(ModelInstance* aModel);
		void RemoveFromScene(PointLight* aLight);
		//void RemoveFromScene(ParticleInstance* aParticle);
		//void RemoveFromScene(SpotLight* aDecal);
		//void RemoveFromScene(Decal* aSpotLight);
		//void RemoveFromScene(SpriteInstance* aSprite);
		//void RemoveFromScene(TextInstance* aText);

		//void RefreshAll(float aAmount);
		void SetSkybox(ModelInstance* aSkybox) { mySkybox = aSkybox;  }


		inline ModelInstance* GetSkybox() { return mySkybox; }
		inline Camera* GetMainCamera() { return myMainCamera; }
		//EnvironmentLight*	GetEnvironmentLight();

		std::vector<ModelInstance*> Cull(Camera* aCamera);
		std::vector<ModelInstance*> Cull(Camera* aCamera, const std::vector<ModelInstance*>& aSelection, float aRangeModifier = 1.f);
		std::vector<ModelInstance*> Cull(const tools::PlaneVolume<float>& aPlaneVolume, const std::vector<ModelInstance*>& aSelection, float aRangeModifier = 1.f);
		std::vector<ModelInstance*> Cull(const tools::Sphere<float>& aBoundingSphere);

		std::array<PointLight*, NUMBEROFPOINTLIGHTS> CullPointLights(ModelInstance* aModel);
		
		//std::vector<ParticleInstance*> GetParticles();
		//const std::vector<SpriteInstance*>& GetSprites();
		//const std::vector<TextInstance*>& GetText();
		//std::vector<PointLight*>& GetPointLights();
		//std::vector<SpotLight*>& GetSpotLights();
		//std::vector<Decal*>& GetDecals();

		inline std::vector<ModelInstance*>::iterator begin() { return myModels.begin(); }
		inline std::vector<ModelInstance*>::iterator end() { return myModels.end(); }

	private:
		std::vector<ModelInstance*> myModels;
		std::vector<PointLight*> myPointLights;
		//std::vector<SpriteInstance*> mySprites;
		//std::vector<SpotLight*> mySpotlights;
		//std::vector<Decal*> myDecals;
		//std::vector<ParticleInstance*> myParticles;
		//std::vector<TextInstance*> myTexts;
		Camera* myMainCamera = nullptr;
		ModelInstance* mySkybox = nullptr;
		//EnvironmentLight* myEnvironmentLight = nullptr;
	};

}

#endif