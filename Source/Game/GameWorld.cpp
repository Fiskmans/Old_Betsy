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
	myScene(nullptr),
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
	myScene = nullptr;
	myGBPhysXPtr = nullptr;

	SAFE_DELETE(myParticleFactory);
	SAFE_DELETE(myGBPhysXColliderFactory);
}

void GameWorld::SystemLoad(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, LightLoader* aLightLoader)
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

void GameWorld::Init(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework)
{
	myScene = aScene;
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

#if USEIMGUI
void GameWorld::ImGuiNode()
{
}

#endif // !_RETAIL

void GameWorld::Update(CommonUtilities::InputHandler& aInputHandler, float aDeltatime)
{
	PerlinNoise noise;
	float now = Tools::GetTotalTime();
	WindSystem::GetInstance().SetBaseWind(V3F((noise.noise(now * 2, now * PI, 5) - 0.5) * 10000, 0, (noise.noise(now * 2, now * PI, 10) - 0.5) * 10000));

#if USEIMGUI
	static bool gameIsPaused = false;
	static bool showBoundingBoxes = false;
	static bool editParticles = false;
	static bool pauseAnimations = false;
	static bool snapCameraOnLoad = true;
	static float expectedLifetime = 10.f;
	static ModelInstance* modelInstance = nullptr;
	static Animator* animator = nullptr;
	static size_t offset;
	static ModelInstance* skybox = nullptr;
	static std::string skyboxPath;
	bool modelViewerOpen = WindowControl::Window("Model Viewer", [&]()
		{
#ifdef _DEBUG
			if (ImGui::BeginTabBar("Viewer"))
			{
				if (ImGui::BeginTabItem("Models"))
				{
					static bool movedCamera = false;
					ImGui::SameLine();
					ImGui::Checkbox("Show Bounding Boxes", &showBoundingBoxes);
					ImGui::Text("LifeTime: %f", modelInstance ? (Tools::GetTotalTime() - modelInstance->GetSpawnTime()) : 0.0f);
					if (ImGui::InputFloat("Expected LifeTime", &expectedLifetime))
					{
						if (modelInstance)
						{
							modelInstance->SetExpectedLifeTime(expectedLifetime);
						}
					}
					ImGui::Checkbox("Snap camera", &snapCameraOnLoad);

					ImGui::Separator();
					ImGui::EndChild();
					if (!movedCamera && modelInstance && snapCameraOnLoad)
					{
						CommonUtilities::Sphere<float> sphere = modelInstance->GetGraphicBoundingSphere();
						if (abs(sphere.Radius() - 1.f) > 0.1f)
						{
							V3F pos = myScene->GetMainCamera()->GetPosition();
							pos.Normalize();
							pos *= sphere.Radius() * 2;
							myScene->GetMainCamera()->SetPosition(pos);
							movedCamera = true;
						}
					}
					if (showBoundingBoxes && modelInstance)
					{
						for (auto& i : modelInstance->GetModelAsset().GetAsModel()->myCollisions)
						{
							DebugDrawer::GetInstance().DrawBoundingBox(i);
						}
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
#endif // _DEBUG
		});
	bool particleEditorOpen = WindowControl::Window("Particle Editor", [&]()
		{
			myScene->RefreshAll(1.f);
			myParticleFactory->EditParticles(myScene);
		});
	WindowControl::Window("GameWorld", [&]()
		{
			if (!particleEditorOpen)
			{
				ImGui::Checkbox("Edit particles", &particleEditorOpen);
			}
			if (!editParticles)
			{
				ImGui::Checkbox("Model Viewer", &modelViewerOpen);
			}

			ImGui::Checkbox("Show Bounding Boxes", &showBoundingBoxes);
			ImGui::Checkbox("Pause Game", &gameIsPaused);
		});

	if (particleEditorOpen != editParticles)
	{
		editParticles = particleEditorOpen;
		static V3F cameraPos;
		if (editParticles)
		{
			myScene->Stash(Scene::StashOp::Push);
			cameraPos = myScene->GetMainCamera()->GetPosition();
			myScene->GetMainCamera()->SetPosition(myScene->GetMainCamera()->GetForward() * -400.f);
		}
		else
		{
			myScene->Stash(Scene::StashOp::Pop);
			myScene->GetMainCamera()->SetPosition(cameraPos);
		}
	}
	if (modelViewerOpen != myIsInModelViewerMode)
	{
		myIsInModelViewerMode = modelViewerOpen;
		static V3F cameraPos;
		if (myIsInModelViewerMode)
		{
			myScene->Stash(Scene::StashOp::Push);
			cameraPos = myScene->GetMainCamera()->GetPosition();
			myScene->GetMainCamera()->SetPosition(myScene->GetMainCamera()->GetForward() * -400.f);
		}
		else
		{
			if (modelInstance)
			{
				myScene->RemoveModel(modelInstance);
				delete modelInstance;
				modelInstance = nullptr;
			}
			SAFE_DELETE(animator);
			myScene->Stash(Scene::StashOp::Pop);
			myScene->GetMainCamera()->SetPosition(cameraPos);
		}
	}

	bool isRunningStandard = !gameIsPaused;

	if (editParticles || myIsInModelViewerMode) //Cameracontrols
	{
#pragma region Cameracontrols
		if (aInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key_Alt))
		{
			static Point lastmp = aInputHandler.GetMousePosition();
			Point mp = aInputHandler.GetMousePosition();
			if (aInputHandler.IsMouseDown(CommonUtilities::InputHandler::Mouse::Mouse_Right))
			{
				const static float speed = 0.001f;

				float totalDiff = 1 / (1 + (mp.x - lastmp.x) * speed) * 1 / (1 + (mp.y - lastmp.y) * speed);
				myScene->GetMainCamera()->SetPosition(myScene->GetMainCamera()->GetPosition() * totalDiff);
			}
			else if (aInputHandler.IsMouseDown(CommonUtilities::InputHandler::Mouse::Mouse_Left))
			{

				V3F pos = myScene->GetMainCamera()->GetPosition();
				float length = pos.Length();
				pos += myScene->GetMainCamera()->GetRight() * length * -static_cast<float>(mp.x - lastmp.x) * 0.001f;
				pos += myScene->GetMainCamera()->GetUp() * length * static_cast<float>(mp.y - lastmp.y) * 0.001f;
				pos = pos.GetNormalized() * length;

				myScene->GetMainCamera()->SetPosition(pos);
				myScene->GetMainCamera()->LookAt(V3F(0, 0, 0));
			}
			else
			{
				lastmp = aInputHandler.GetMousePosition();
			}
			lastmp = mp;
		}
#pragma endregion
	}
	if (myIsInModelViewerMode)
	{
		static V3F lightOffset(-67, -34, 79);
		//ImGui::DragFloat3("Light arrow position", &lightOffset.x);
		if (myScene->GetEnvironmentLight())
		{
			DebugDrawer::GetInstance().DrawDirection(
				myScene->GetMainCamera()->GetPosition() +
				myScene->GetMainCamera()->GetForward() * lightOffset.z +
				myScene->GetMainCamera()->GetUp() * lightOffset.y +
				myScene->GetMainCamera()->GetRight() * lightOffset.x,
				-myScene->GetEnvironmentLight()->myDirection);
		}
	}
	else if (editParticles) {/*NO-OP*/ }
	else if (isRunningStandard)
#endif // !USEIMGUI
	{

		{
			EnvironmentLight* env = myScene->GetEnvironmentLight();
			if (env)
			{
				env->myShadowCorePosition = V3F(0, 0, 0);
			}
		}
	}


	CommonUtilities::Vector3<float> movement = { 0.f, 0.f, 0.f };
	CommonUtilities::Vector3<float> rotation = { 0.f, 0.f, 0.f };

	{
		if (aInputHandler.IsKeyDown(aInputHandler.Key_W))
		{
			movement.z += myFreecamSpeed * aDeltatime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_S))
		{
			movement.z -= myFreecamSpeed * aDeltatime;
		}

		if (aInputHandler.IsKeyDown(aInputHandler.Key_D))
		{
			movement.x += myFreecamSpeed * aDeltatime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_A))
		{
			movement.x -= myFreecamSpeed * aDeltatime;
		}

		if (aInputHandler.IsKeyDown(aInputHandler.Key_Space))
		{
			movement.y += myFreecamSpeed * aDeltatime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_Shift))
		{
			movement.y -= myFreecamSpeed * aDeltatime;
		}

		if (aInputHandler.IsKeyDown(aInputHandler.Key_Q))
		{
			rotation.y -= myFreecamRotationSpeed * aDeltatime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_E))
		{
			rotation.y += myFreecamRotationSpeed * aDeltatime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_Z))
		{
			rotation.x -= myFreecamRotationSpeed * aDeltatime;
		}
		if (aInputHandler.IsKeyDown(aInputHandler.Key_X))
		{
			rotation.x += myFreecamRotationSpeed * aDeltatime;
		}
		myScene->GetMainCamera()->Move(movement);
		myScene->GetMainCamera()->Rotate(rotation);
	}
}

