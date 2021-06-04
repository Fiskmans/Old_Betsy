#pragma once

#include "CommonUtilities\Sphere.hpp"


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

class Scene
{
public:
	~Scene();

	void Update(float aDeltaTime);

	void AddToScene(ModelInstance* aModel);
	void AddToScene(SpotLight* aSpotLight);
	void AddToScene(Decal* aDecal);
	void AddSprite(SpriteInstance* aSprite);
	void AddText(TextInstance* aText);
	void AddInstance(Camera* aCamera);
	void AddInstance(ParticleInstance* aParticle);
	void RemoveModel(ModelInstance* aModel);
	void SetMainCamera(Camera* aCamera);
	void SetEnvironmentLight(EnvironmentLight* aLight);
	void AddPointLight(PointLight* aLight);
	void RemovePointLight(PointLight* aLight);
	void RemoveFrom(ParticleInstance* aParticle);
	void RemoveFrom(SpotLight* aDecal);
	void RemoveFrom(Decal* aSpotLight);
	void RemoveSprite(SpriteInstance* aSprite);
	void RemoveText(TextInstance* aText);
	bool Contains(ParticleInstance* aParticle);
	bool Contains(ModelInstance* aModel);
	void RemoveAll();
	void RefreshAll(float aAmount);
	void SetSkybox(ModelInstance* aSkybox);

	ModelInstance* GetInstance(const FRay& aRay);
	std::vector<ModelInstance*> GetIntersections(const FRay& aRay);

#if USEIMGUI
	enum class StashOp
	{
		Push,
		Pop
	};
	void Stash(StashOp aOP);
#endif // USEIMGUI

	std::vector<ParticleInstance*> GetParticles();

	ModelInstance* GetSkybox();
	Camera* GetMainCamera();
	EnvironmentLight* GetEnvironmentLight();
	std::vector<PointLight*>& GetPointLights();
	std::vector<SpotLight*>& GetSpotLights();
	std::vector<Decal*>& GetDecals();

	std::vector<ModelInstance*> Cull(Camera* aCamera, bool aShouldSort = true);
	std::vector<ModelInstance*> Cull(Camera* aCamera, std::vector<ModelInstance*>& aSelection, bool aShouldSort = true, float aRangeModifier = 1.f);
	std::vector<ModelInstance*> Cull(const CommonUtilities::PlaneVolume<float>& aPlaneVolume, std::vector<ModelInstance*>& aSelection, const V3F& aCameraPos, bool aShouldSort = true, float aRangeModifier = 1.f);
	std::vector<ModelInstance*> Cull(const CommonUtilities::Sphere<float>& aBoundingSphere);

	std::array<PointLight*, NUMBEROFPOINTLIGHTS> CullPointLights(ModelInstance* aModel);

	const std::vector<SpriteInstance*>& GetSprites();
	const std::vector<TextInstance*>& GetText();

	std::vector<ModelInstance*>::iterator begin();
	std::vector<ModelInstance*>::iterator end();

private:
	std::vector<ModelInstance*> myPreCull;
	std::vector<ModelInstance*> myModels;
	std::vector<SpriteInstance*> mySprites;
	std::vector<Camera*> myCameras;
	std::vector<PointLight*> myPointLights;
	std::vector<SpotLight*> mySpotlights;
	std::vector<Decal*> myDecals;
	std::vector<ParticleInstance*> myParticles;
	std::vector<TextInstance*> myTexts;
	Camera* myMainCamera = nullptr;
	ModelInstance* mySkybox = nullptr;
	EnvironmentLight* myEnvironmentLight = nullptr;

};

