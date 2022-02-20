#include <pch.h>
#include "GameWorld.h"
#include "Camera.h"
#include "InputHandler.h"

#include <ModelLoader.h>

#include "Model.h"
#include "Animator.h"

#include "ParticleFactory.h"
#include <SpriteFactory.h>
#include <SpriteInstance.h>

#include "AudioManager.h"

#include "TextFactory.h"

#include "GBPhysX.h"

#include "TextInstance.h"

#include "GamlaBettan\PathFinder.h"
#include "Tools\perlin_noise.h"

#include <Environmentlight.h>
#include "ParticleInstance.h"
#include <SpotLight.h>
#include <SpotLightFactory.h>
#include "PointLight.h"

#include "DirectXTK\Inc\SpriteBatch.h"
#include "DirectX11Framework.h"

#include "AssetManager.h"

#include "GamlaBettan\ModelInstance.h"
#include "GamlaBettan\RenderScene.h"
#include "GamlaBettan\EntityManager.h"

#include "GamlaBettan\MeshComponent.h"
#include "GamlaBettan\FreeCam.h"


#include "Game\BounceComponent.h"

GameWorld::GameWorld(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX)
	: myParticleFactory(nullptr)
	, myGBPhysXPtr(nullptr)
	, mySpriteFactory(nullptr)
	, myPlayer(0)
	, myTerrain(aFramework, aGBPhysX, { 16, 16, 16 }, {8_m, 8_m, 8_m})
{
	myWindowSize.x = 1920;
	myWindowSize.y = 1080;
	mySpriteFactory = aSpriteFactory;

	aAudioManager->SetMasterVolume(40.0f);
	aAudioManager->Register2DMasterObject(0);

	myParticleFactory = new ParticleFactory;
	if (!myParticleFactory->Init(aFramework))
	{
		throw std::exception("Failed to initialize GameWorld");
	}
	myGBPhysXPtr = aGBPhysX;

	SetupWorld();
}

GameWorld::~GameWorld()
{
	myGBPhysXPtr = nullptr;

	SAFE_DELETE(myParticleFactory);
}

void GameWorld::SetupWorld()
{
	myPlayer = EntityManager::GetInstance().MakeEntity();
	//EntityManager::GetInstance().Retrieve(myPlayer)->AddComponent<MeshComponent>("Quaternius/Medieval/Buildings/House_1.fbx");
	//EntityManager::GetInstance().Retrieve(myPlayer)->AddComponent<BounceComponent>();
	EntityManager::GetInstance().Retrieve(myPlayer)->AddComponent<FreeCam>();
}

#if USEIMGUI
void GameWorld::ImGuiNode()
{
}
#endif // !_RETAIL

void GameWorld::Update()
{
	PerlinNoise noise;

#if USEIMGUI
	static bool showBoundingBoxes = false;
	static bool pauseAnimations = false;
	static bool snapCameraOnLoad = true;
	static float expectedLifetime = 10.f;
	static ModelInstance* modelInstance = nullptr;
	static Animator* animator = nullptr;
	static size_t offset;
	static ModelInstance* skybox = nullptr;
	static std::string skyboxPath;
	
	WindowControl::Window("GameWorld", [&]()
		{
			ImGui::Checkbox("Show Bounding Boxes", &showBoundingBoxes);
		});

	myTerrain.Imgui();
#endif // !USEIMGUI

	{
		PERFORMANCETAG("Terrain");
		myTerrain.Update();
	}

	{
		EnvironmentLight* env = RenderScene::GetInstance().GetEnvironmentLight();
		if (env)
		{
			env->myShadowCorePosition = EntityManager::GetInstance().Retrieve(myPlayer)->GetPosition();
		}
	}
}

