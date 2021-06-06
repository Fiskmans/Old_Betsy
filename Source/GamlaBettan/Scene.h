#pragma once

#include "CommonUtilities\Sphere.hpp"
#include "CommonUtilities\Singleton.hpp"


class ModelInstance;
class SpriteInstance;
class Camera;
struct EnvironmentLight;
class PointLight;
class ParticleInstance;
class TextInstance;
class ProgressBar;
struct SpotLight;
struct Decal;

class Scene : public CommonUtilities::Singleton<Scene>
{
public:
	~Scene();

	void Update(float aDeltaTime);

	void AddToScene(ModelInstance* aModel);
	void AddToScene(SpotLight* aSpotLight);
	void AddToScene(Decal* aDecal);
	void AddToScene(SpriteInstance* aSprite);
	void AddToScene(TextInstance* aText);
	void AddToScene(ParticleInstance* aParticle);
	void AddToScene(PointLight* aLight);

	void SetMainCamera(Camera* aCamera);
	void SetEnvironmentLight(EnvironmentLight* aLight);

	void RemoveFromScene(ModelInstance* aModel);
	void RemoveFromScene(PointLight* aLight);
	void RemoveFromScene(ParticleInstance* aParticle);
	void RemoveFromScene(SpotLight* aDecal);
	void RemoveFromScene(Decal* aSpotLight);
	void RemoveFromScene(SpriteInstance* aSprite);
	void RemoveFromScene(TextInstance* aText);

	void RemoveAll();
	void RefreshAll(float aAmount);
	void SetSkybox(ModelInstance* aSkybox);

	ModelInstance*		GetSkybox();
	Camera*				GetMainCamera();
	EnvironmentLight*	GetEnvironmentLight();

	std::vector<ModelInstance*> Cull(Camera* aCamera, bool aShouldSort = true);
	std::vector<ModelInstance*> Cull(Camera* aCamera, std::vector<ModelInstance*>& aSelection, bool aShouldSort = true, float aRangeModifier = 1.f);
	std::vector<ModelInstance*> Cull(const CommonUtilities::PlaneVolume<float>& aPlaneVolume, std::vector<ModelInstance*>& aSelection, const V3F& aCameraPos, bool aShouldSort = true, float aRangeModifier = 1.f);
	std::vector<ModelInstance*> Cull(const CommonUtilities::Sphere<float>& aBoundingSphere);

	std::array<PointLight*, NUMBEROFPOINTLIGHTS> CullPointLights(ModelInstance* aModel);

	std::vector<ParticleInstance*> GetParticles();
	const std::vector<SpriteInstance*>& GetSprites();
	const std::vector<TextInstance*>& GetText();
	std::vector<PointLight*>& GetPointLights();
	std::vector<SpotLight*>& GetSpotLights();
	std::vector<Decal*>& GetDecals();

	std::vector<ModelInstance*>::iterator begin();
	std::vector<ModelInstance*>::iterator end();

private:
	std::vector<ModelInstance*> myPreCull;
	std::vector<ModelInstance*> myModels;
	std::vector<SpriteInstance*> mySprites;
	std::vector<PointLight*> myPointLights;
	std::vector<SpotLight*> mySpotlights;
	std::vector<Decal*> myDecals;
	std::vector<ParticleInstance*> myParticles;
	std::vector<TextInstance*> myTexts;
	Camera* myMainCamera = nullptr;
	ModelInstance* mySkybox = nullptr;
	EnvironmentLight* myEnvironmentLight = nullptr;

};

