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

GameWorld::GameWorld() :
	myParticleFactory(nullptr),
	myGBPhysXPtr(nullptr),
	myGBPhysXColliderFactory(nullptr),
	mySpriteFactory(nullptr),
	Observer(
		{
			MessageType::NewLevelLoaded,
			MessageType::WindowResize,
			MessageType::EnemyDied,
			MessageType::TriggerEvent,
			MessageType::FadeInComplete,
			MessageType::StartDialogue,
			MessageType::DialogueOver,
			MessageType::RespawnTrader,
			MessageType::DespawnTrader,
			MessageType::RiverCreated,
			MessageType::FadeOut,
			MessageType::FadeOutComplete,
			MessageType::CreateSeed,
			MessageType::RequestUISetupPtrs,
			MessageType::StartInGameAudio,
			MessageType::DialogueOver
		}
	)
{
}

GameWorld::~GameWorld()
{
	myGBPhysXPtr = nullptr;

	SAFE_DELETE(myParticleFactory);
	SAFE_DELETE(myGBPhysXColliderFactory);
}

void GameWorld::SystemLoad(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, LightLoader* aLightLoader)
{
	myWindowSize.x = 1920;
	myWindowSize.y = 1080;
	mySpriteFactory = aSpriteFactory;

	aAudioManager->SetMasterVolume(40.0f);
	aAudioManager->Register2DMasterObject(0);

	myParticleFactory = new ParticleFactory;

	myGBPhysXPtr = aGBPhysX;


	myParticleFactory->Init(aFramework);
}

void GameWorld::Init(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework)
{
	mySpriteFactory = aSpriteFactory;
}

V3F ClosestPointOnLine(V3F aLineStart, V3F aLineEnd, V3F aPoint)
{
	V3F AB = aLineEnd - aLineStart;
	V3F AP = aPoint - aLineStart;

	float magnitudeAB = AB.LengthSqr();
	float ABAPproduct = AP.Dot(AB);
	float distance = ABAPproduct / magnitudeAB;

	V3F pos;
	if (distance < 0)
	{
		return aLineStart;
	}
	else if (distance > 1)
	{
		return aLineEnd;
	}
	else
	{
		return aLineStart + AB * distance;
	}
}


void GameWorld::RecieveMessage(const Message& aMessage)
{
}

void GameWorld::FreecamMovement(CommonUtilities::InputHandler& aInputHandler, float aDeltaTime)
{
	if (aInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key_Alt))
	{
		Camera* mainCam = Scene::GetInstance().GetMainCamera();

		static Point lastmp = aInputHandler.GetMousePosition();
		Point mp = aInputHandler.GetMousePosition();
		if (aInputHandler.IsMouseDown(CommonUtilities::InputHandler::Mouse::Mouse_Right))
		{
			const static float speed = 0.001f;

			float totalDiff = 1 / (1 + (mp.x - lastmp.x) * speed) * 1 / (1 + (mp.y - lastmp.y) * speed);
			mainCam->SetPosition(mainCam->GetPosition() * totalDiff);
		}
		else if (aInputHandler.IsMouseDown(CommonUtilities::InputHandler::Mouse::Mouse_Left))
		{
			V3F pos = mainCam->GetPosition();
			float length = pos.Length();
			pos += mainCam->GetRight() * length * -static_cast<float>(mp.x - lastmp.x) * 0.001f;
			pos += mainCam->GetUp() * length * static_cast<float>(mp.y - lastmp.y) * 0.001f;
			pos = pos.GetNormalized() * length;

			mainCam->SetPosition(pos);
			mainCam->LookAt(V3F(0, 0, 0));
		}
		else
		{
			lastmp = aInputHandler.GetMousePosition();
		}
		lastmp = mp;
	}
	else
	{
		CommonUtilities::Vector3<float> movement = { 0.f, 0.f, 0.f };
		CommonUtilities::Vector3<float> rotation = { 0.f, 0.f, 0.f };
		if (aInputHandler.IsKeyDown(aInputHandler.Key_W))
		{
			movement.z += myFreecamSpeed * aDeltaTime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_S))
		{
			movement.z -= myFreecamSpeed * aDeltaTime;
		}

		if (aInputHandler.IsKeyDown(aInputHandler.Key_D))
		{
			movement.x += myFreecamSpeed * aDeltaTime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_A))
		{
			movement.x -= myFreecamSpeed * aDeltaTime;
		}

		if (aInputHandler.IsKeyDown(aInputHandler.Key_Space))
		{
			movement.y += myFreecamSpeed * aDeltaTime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_Shift))
		{
			movement.y -= myFreecamSpeed * aDeltaTime;
		}

		if (aInputHandler.IsKeyDown(aInputHandler.Key_Q))
		{
			rotation.y -= myFreecamRotationSpeed * aDeltaTime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_E))
		{
			rotation.y += myFreecamRotationSpeed * aDeltaTime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_Z))
		{
			rotation.x -= myFreecamRotationSpeed * aDeltaTime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_X))
		{
			rotation.x += myFreecamRotationSpeed * aDeltaTime;
		}
		Camera* mainCam = Scene::GetInstance().GetMainCamera();

		mainCam->Move(movement);
		mainCam->Rotate(rotation);
	}
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
	static bool freeCam = false;
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
			ImGui::Checkbox("Free Camera", &freeCam);
		});

	if (freeCam)
	{
		FreecamMovement(aInputHandler, aDeltaTime);
	}
	else
#endif // !USEIMGUI
	{
		{
			EnvironmentLight* env = Scene::GetInstance().GetEnvironmentLight();
			if (env)
			{
				env->myShadowCorePosition = myPlayer.GetPosition();
			}
		}
	}
}

