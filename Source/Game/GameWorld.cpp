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

#include <WindSystem.h>

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
#include "GamlaBettan\Scene.h"
#include "GamlaBettan\EntityManager.h"

#include "GamlaBettan\MeshComponent.h"
#include "GamlaBettan\FreeCam.h"


#include "Game\BounceComponent.h"

GameWorld::GameWorld() :
	myParticleFactory(nullptr),
	myGBPhysXPtr(nullptr),
	myGBPhysXColliderFactory(nullptr),
	mySpriteFactory(nullptr)
{
}

GameWorld::~GameWorld()
{
	myGBPhysXPtr = nullptr;

	SAFE_DELETE(myParticleFactory);
	SAFE_DELETE(myGBPhysXColliderFactory);
}

bool GameWorld::Init(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, LightLoader* aLightLoader)
{
	myWindowSize.x = 1920;
	myWindowSize.y = 1080;
	mySpriteFactory = aSpriteFactory;

	aAudioManager->SetMasterVolume(40.0f);
	aAudioManager->Register2DMasterObject(0);

	myParticleFactory = new ParticleFactory;
	if (!myParticleFactory->Init(aFramework))
	{
		return false;
	}
	myGBPhysXPtr = aGBPhysX;

	SetupWorld();
}

void GameWorld::SetupWorld()
{
	myPlayer = EntityManager::GetInstance().Get();
	EntityManager::GetInstance().Retrieve(myPlayer)->AddComponent<MeshComponent>("Quaternius/Medieval/Buildings/House_1.fbx");
	EntityManager::GetInstance().Retrieve(myPlayer)->AddComponent<BounceComponent>();
	EntityManager::GetInstance().Retrieve(myPlayer)->AddComponent<FreeCam>();
}

#if USEIMGUI
void GameWorld::ImGuiNode()
{
}

#endif // !_RETAIL

void GameWorld::Update(CommonUtilities::InputHandler& aInputHandler, float aDeltaTime)
{
	PerlinNoise noise;
	float now = Tools::GetTotalTime();
	WindSystem::GetInstance().SetBaseWind(V3F((noise.noise(now * 2, now * PI, 5) - 0.5) * 10000, 0, (noise.noise(now * 2, now * PI, 10) - 0.5) * 10000));

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

#endif // !USEIMGUI


	{
		EnvironmentLight* env = Scene::GetInstance().GetEnvironmentLight();
		if (env)
		{
			env->myShadowCorePosition = EntityManager::GetInstance().Retrieve(myPlayer)->GetPosition();
		}
	}
}

