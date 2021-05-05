#pragma once
#include "ImGuiPackage.h"
class CinematicState;
class ModelLoader;
class Scene;
class ParticleFactory;
class SpriteFactory;

class CinematicEditor
{
public:
	void Init(ModelLoader* aLoader, Scene* aScene, ParticleFactory* aPartFactory, SpriteFactory* aSpriteFactory);
	void Init(CinematicState* aState);
	bool Run();

private:
	enum class EventType
	{
		CameraPan,
		ParticleBurst,
		ScreenFade,
		MoveObject,
		PlayAnimation,
		LookAt
	};

	void Imgui();

	void Load(const std::string& aFile);
	void Save();

	void AddEvent(EventType aType);

	void Test();

	void FlattenPanning();
	void VerifyTiming();

	Scene* myScene = nullptr;
	ModelLoader* myLoader = nullptr;
	ParticleFactory* myPartFactory = nullptr;

	CinematicState* myState = nullptr;
	std::string myLoaded;
};

