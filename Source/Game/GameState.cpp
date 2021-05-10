#pragma once
#include "pch.h"
#include "GameState.h"
#include <GraphicEngine.h>
#include <Scene.h>
#include <ModelLoader.h>
#include <SpriteFactory.h>
#include "ForwardRenderer.h"
#include "SpriteRenderer.h"
#include "InputManager.h"
#include "PostMaster.hpp"
#include <assert.h>
#include <fstream>
#include <Skybox.h>
#include <imgui.h>

#include "AudioManager.h"

#include "DataStructs.h"
#include <StringManipulation.h>
#include <DebugDrawer.h>
#include <algorithm>

//TODO Move to a better place
#include "ModelInstance.h"
#include "Model.h"
#include "CameraFactory.h"
#include "Camera.h"
#include "LightLoader.h"
#include "Environmentlight.h"
#include "PointLight.h"
#include "VideoState.h"
#include <Random.h>
#include "VideoState.h"
#include "NodePollingStation.h"
#include "CNodeInstance.h"
#include "CinematicState.h"
#include <functional>
#include <DirectX11Framework.h>
#include "SpriteInstance.h"
#include <random>
#include <DirectXTK\Inc\ScreenGrab.h>
#include <DirectXTK/Inc/DDSTextureLoader.h>
#include <FullscreenRenderer.h>

#include <PathFinder.h>
#include "PathFinderData.h"

#include <GBPhysX.h>

#if USEIMGUI
#include <experimental/filesystem>
#include "ImGuiPackage.h"
#include <WindowControl.h>
#endif // _DEBUG

#ifdef _DEBUG
#include "DebugTools.h"
#endif
#if USEFILEWATHCER
#include <ShaderCompiler.h>
#endif

#if ENABLESCREEPS
#include "ScreepsState.h"
#endif

#include "AssetManager.h"

//Network
#include <DeathSpotMessage.h>

#include "Complex.h"
#include "Fourier.h"

GameState::GameState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop),
	myScene(nullptr),
	myModelLoader(nullptr),
	mySpriteFactory(nullptr),
	mySkybox(nullptr),
	myLightLoader(nullptr)
{
	myGameWorld = new GameWorld();
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
	myInputManager = nullptr;
	PostMaster::GetInstance()->Subscribe(MessageType::FadeOutComplete, this);
	PostMaster::GetInstance()->Subscribe(MessageType::FinnishGame, this);
	PostMaster::GetInstance()->Subscribe(MessageType::PlayerDied, this);
	PostMaster::GetInstance()->Subscribe(MessageType::EnableScreeps, this);
	PostMaster::GetInstance()->Subscribe(MessageType::ChangeLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::NextLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::BulletHit, this);
	PostMaster::GetInstance()->Subscribe(MessageType::LoadLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::UnloadLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::AnnounceDeathMarker, this);
	PostMaster::GetInstance()->Subscribe(MessageType::ConnectedToServer, this);
	PostMaster::GetInstance()->Subscribe(MessageType::WinGameAndAlsoLife, this);
	PostMaster::GetInstance()->Subscribe(MessageType::GoToSleep, this);
	myCurrentLevel = "";
	myIsMain = true;
	myCurrentLevelindex = 0;
	myGraphManager.Load();
	myNodePollingStation = nullptr;
	myFinnishGameAfterFadeOut = false;
	myQueuedPartialLevel.myIsComplete = false;
	myHasRenderedAtleastOneFrame = false;
}

GameState::~GameState()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::FadeOutComplete, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::FinnishGame, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::PlayerDied, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::EnableScreeps, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::ChangeLevel, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::NextLevel, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::BulletHit, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::LoadLevel, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::UnloadLevel, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::AnnounceDeathMarker, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::ConnectedToServer, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::WinGameAndAlsoLife, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::GoToSleep, this);

#if USEFILEWATHCER
	myWatcher.UnRegister(myMetricHandle);
#endif // !_RETAIL
#ifdef _DEBUG
	DebugTools::FileList = nullptr;
#endif
	SAFE_DELETE(myGameWorld);
	SAFE_DELETE(myScene);
	if (myGBPhysX)
	{
		myGBPhysX->GBCleanUpPhysics();
	}
	SAFE_DELETE(myGBPhysX);
}

bool GameState::Init(WindowHandler* aWindowHandler, InputManager* aInputManager, ModelLoader* aModelLoader, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myIsGameWon = false;
	myIsMain = true;

	myContext = aFramework->GetContext();
	myFramework = aFramework;
	myModelLoader = aModelLoader;
	mySpriteFactory = aSpritefactory;
	myLightLoader = aLightLoader;
	myInputManager = aInputManager;
	myAudioManager = aAudioManager;
	myWindowHandler = aWindowHandler;
	myGraphManager.LoadTreeFromFile();
	myGBPhysX = new GBPhysX();

	mySpotlightFactory.Init(aFramework->GetDevice());
	myDecalFactory.Init(aFramework->GetDevice());

	if (!myGBPhysX->GetGBPhysXActive())
	{
		myGBPhysX->GBInitPhysics(false);
	}
	else
	{
		SYSERROR("Tried to Init GBPhysX when it was already running", "");
		throw std::exception("Tried to Init GBPhysX when it was already running");
	}

	CreateWorld(aWindowHandler, aFramework, aAudioManager, aSpriteRenderer);


#ifdef _DEBUG
	DebugTools::myLightLoader = myLightLoader;

#endif // _DEBUG


#if !BAKEVALUES
	MetricHandler::Load(METRICFILE);
#if USEFILEWATHCER
	myMetricHandle = myWatcher.RegisterCallback(METRICFILE, MetricHandler::Load);
#endif // !_RETAIL
#endif


#if DONETWORK
	myClient.SetCallback(std::bind(&GameState::RecieveNetMessage, this, std::placeholders::_1));
	myClient.Start();
#endif
	return !!aModelLoader;
}

void GameState::Render(CGraphicsEngine* aGraphicsEngine)
{
	if (!myIsGameWon)
	{
		PathFinder::GetInstance().DrawDebug();

		aGraphicsEngine->RenderFrame(myScene);
	}
	myHasRenderedAtleastOneFrame = true;
}

void GameState::LoadLevel(const int& aLevel)
{
	myCurrentLevelindex = aLevel;

#if DEMOSCENE
	LoadLevel("");
	return;
#endif
	//TODO: --AIUPPGIFT--
	if (myLevelSequence.empty())
	{
		EnvironmentLight* light = myLightLoader->LoadLight("Data/Textures/Skyboxes/skyBox_Level2_light.dds");
		if (light)
		{
			light->myDirection = V3F(1, 1, 1);
			light->myIntensity = 1;
			light->myColor = V3F(1, 1, 1);

			EnvironmentLight* oldLight = myScene->GetEnvironmentLight();
			myScene->SetEnvironmentLight(light);
			SAFE_DELETE(oldLight);
		}

		return;
	}

	if (myCurrentLevelindex >= myLevelSequence.size() || myCurrentLevelindex < 0)
	{
		SYSERROR("Level doesnt exist", std::to_string(myCurrentLevelindex));
		return;
	}

	std::string levelToLoad = myLevelSequence[myCurrentLevelindex];
	//myLevelSequence.pop();
	if (!LoadLevel(levelToLoad))
	{
		SYSERROR("Failed to Load level: ", levelToLoad);
	}

	Message levelNumberMessage;
	levelNumberMessage.myMessageType = MessageType::CurrentLevel;
	levelNumberMessage.myIntValue = myCurrentLevelindex;
	SendMessages(levelNumberMessage);

}

void GameState::PreSetup(const float aDeltaTime)
{
	if (myQueuedPartialLevel.myIsComplete)
	{
		MergeQueuedLevelPartially(0.007f);
	}
	else
	{
		if (myAsyncPartialLevel.valid() && myAsyncPartialLevel.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
		{
			myQueuedPartialLevel = myAsyncPartialLevel.get();
			myAsyncPartialLevel = std::future<LevelParseResult>();
		}
	}
}


void GameState::RecieveNetMessage(NetMessage* aMessage)
{
	switch (aMessage->myType)
	{
	case NetMessage::Type::DeathMessage:
	{
		DeathSpotMessage* death = reinterpret_cast<DeathSpotMessage*>(aMessage);
		Message mess;
		mess.myMessageType = MessageType::DeathMarkerRecieved;
		mess.myData = &death->myPosition;
		PostMaster::GetInstance()->SendMessages(mess);
	}
	break;
	default:
		break;
	}
}

void GameState::RunImGui(float aDeltatime)
{
#if  USEIMGUI
	PathFinder::GetInstance().Imgui();
	// ImGui
	ModelInstance* toRemove = nullptr;


	static bool ShowGizmo = false;
	static bool editGraphs = false;
	static bool drawFrustums = false;
	if (drawFrustums)
	{
		for (auto i : myScene->GetDecals())
		{
			DebugDrawer::GetInstance().DrawFrustum(i->myCamera->GenerateFrustum());
		}
	}
	if (GetAsyncKeyState('O'))
	{
		myScene->GetDecals().clear();
	}

	static V2F GizmoPosition = { 0.f,16.f };
#ifdef _DEBUG
	DebugTools::FileList = &myFoundFiles;
#endif // _DEBUG
	{
		PERFORMANCETAG("Gamestate imgui");
		WindowControl::Window("Game", [&]()
			{

				ImGui::Checkbox("Visualize decals", &drawFrustums);
				ImGui::Checkbox("Edit graphs", &editGraphs);
				if (editGraphs)
				{

					myGraphManager.PreFrame(aDeltatime);
					myGraphManager.ConstructEditorTreeAndConnectLinks();
					myGraphManager.PostFrame();
				}
				ImGui::Checkbox("Show Gizmo", &ShowGizmo);
				if (ShowGizmo)
				{
					ImGui::SameLine();
					static bool moveGizmo = false;
					ImGui::Checkbox("Move Gizmo", &moveGizmo);
					if (moveGizmo)
					{
						if (ImGui::Begin("Gizmo", &moveGizmo, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
						{
							ImGui::DragFloat2("Position", &GizmoPosition.x, 0.1f, -70.f, 70.f);
						}
						ImGui::End();
					}
				}



				ImGui::Separator();

				EnvironmentLight* light = myScene->GetEnvironmentLight();
				if (light)
				{
					ImGui::ColorEdit3("Light Color", &light->myColor.x);
					ImGui::DragFloat3("Light Direction", &light->myDirection.x, 0.01f);
					ImGui::DragFloat("Light Intensity", &light->myIntensity, 0.01f);
				}
				static float fov = 81;
				static float fov2 = 65;
				static float lastFov;
				static float lastFov2;
				ImGui::DragFloat("Camera FOV", &fov, 0.1f, 0.f, 170.f);
				ImGui::DragFloat("Camera Weapon FOV", &fov2, 0.1f, 0.f, 190.f);
				if (fov != lastFov)
				{
					myScene->GetMainCamera()->SetFov(fov);
					lastFov = fov;
				}
				if (fov2 != lastFov2)
				{
					myScene->GetMainCamera()->SetFov(fov2, true);
					lastFov2 = fov2;
				}

				if (ImGui::CollapsingHeader("Scene"))
				{
					if (ImGui::Button("Remove all"))
					{
						myScene->RemoveAll();
					}
					if (ImGui::TreeNode("Meshes"))
					{
						for (ModelInstance* i : *myScene)
						{
							if (ImGui::TreeNode(i->GetFriendlyName().c_str()))
							{
								if (i->ImGuiNode(myFoundFiles, myScene->GetMainCamera()))
								{
									toRemove = i;
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Lights"))
					{
						int counter = 0;
						for (PointLight* i : myScene->GetPointLights())
						{
							ImGui::PushID(counter);
							ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&i->color));
							ImGui::DragFloat3("Position", reinterpret_cast<float*>(&i->position));
							ImGui::DragFloat("Intensity", &i->intensity);
							ImGui::DragFloat("Range", &i->range);
							ImGui::Separator();
							ImGui::PopID();
							counter++;
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Sprites"))
					{
						size_t counter = 0;
						for (auto& SpriteInstance : myScene->GetSprites())
						{
							ImGui::PushID(counter);
							if (ImGui::TreeNode("node", "Sprite: " PFSIZET, counter))
							{
								SpriteInstance->ImGui();
								ImGui::Separator();
								ImGui::TreePop();
							}
							ImGui::PopID();
							counter++;
						}
						ImGui::TreePop();
					}
				}

				ImGuiNodePackage pack;
				pack.myFileList = &myFoundFiles;
				pack.myMainCamera = myScene->GetMainCamera();
				myGameWorld->ImGuiNode(pack);
				if (ImGui::CollapsingHeader("Tools"))
				{
					if (ImGui::Button("Clear log"))
					{
						system("cls");
					}

					if (ImGui::Button("Reload lists"))
					{
						SearchForFiles();
					}
					if (ImGui::TreeNode("Levels"))
					{
						static bool removeOnReload = true;
						static bool resetOnReload = true;
						ImGui::Checkbox("Remove all on load", &removeOnReload);
						ImGui::Checkbox("Reset camera on load", &resetOnReload);

						for (auto& i : myFoundFiles[".lvl"])
						{

							ImGui::PushID(i.c_str());
							if (ImGui::Button("Set as default"))
							{
								SetDefaultLevel(i);
							}
							ImGui::SameLine();
							if (ImGui::Button("Load"))
							{
								if (removeOnReload)
								{
									myGameWorld->ClearWorld();
									myScene->RemoveAll();
								}
								if (resetOnReload)
								{
									myScene->GetMainCamera()->SetPosition({ 0,0,0 });
									myScene->GetMainCamera()->SetRotation(CommonUtilities::Matrix3x3<float>());
								}
								LoadLevel(i);
							}
							ImGui::SameLine();
							ImGui::Text(i.c_str());
							ImGui::PopID();

						}
						ImGui::TreePop();
					}
				}
				if (ImGui::CollapsingHeader("Info"))
				{
					CommonUtilities::Vector3<float> pos = myScene->GetMainCamera()->GetPosition();
					ImGui::BulletText("Camera position X:%.2f Y:%.2f Z:%.2f", pos.x, pos.y, pos.z);
					ImGui::BulletText("Mouse screen position X:%.2f Y:%.2f", myInputManager->GetMouseNomalizedX(), myInputManager->GetMouseNomalizedY());
#if _DEBUG
					V3F mpWorld = PathFinder::GetInstance().FindPoint(*DebugTools::LastKnownMouseRay);
					if (mpWorld == V3F(0, 0, 0))
					{
						ImGui::BulletText("Mouse world position Outside of navmesh");
					}
					else
					{
						ImGui::BulletText("Mouse world position X:%.2f Y:%.2f Z:%.2f", mpWorld.x, mpWorld.y, mpWorld.z);
					}
#endif // _DEBUG
				}
			});
		Logger::RapportWindow();
	}

	//GAMEWORLD UPDATE --------------------------------------

	{
		PERFORMANCETAG("Gameworld update")
			myGameWorld->Update(*myInputManager->GetInputHandler(), aDeltatime);
	}

	WindowControl::Window("Components", []()
		{
			for (auto& i : GetComponentsInUse())
			{
				ImGui::Text(i.first + 6); // skip 'class ' part of c string
				ImGui::SameLine();
				ImGui::Förloppsindikator(float(i.second.first) / i.second.second, ImVec2(-1, 0), std::string(std::to_string(i.second.first) + "/" + std::to_string(i.second.second)).c_str());
			}
		});


	// Gizmo ------------------------------------------------

	if (ShowGizmo)
	{
		Camera* camera = myScene->GetMainCamera();
		V3F pos = camera->GetPosition() + camera->GetForward() * 40.f + camera->GetUp() * GizmoPosition.y + camera->GetRight() * GizmoPosition.x;
		DebugDrawer::GetInstance().DrawGizmo(pos, 2);
	}

	// Model Remove -----------------------------------------

	if (toRemove)
	{
		myScene->RemoveModel(toRemove);
	}

	// Flush Model Changes ----------------------------------

	Model::FlushChanges();


#endif // !_RETAIL
}

void GameState::SetDefaultLevel(const std::string& aLevel)
{
	std::ofstream outFile;
	outFile.open("defaultLevel.unversioned");
	outFile << aLevel;
}

void GameState::LoadDefaultLevel()
{
#ifdef _DEBUG
	std::ifstream inFile;
	inFile.open("defaultLevel.unversioned");
	if (inFile)
	{
		std::string levelFile;
		std::getline(inFile, levelFile);
		LoadLevel(levelFile);
		return;
	}
#endif
	LoadLevel("Data/Levels/LVL1_A_Harbour.lvl");
}

bool GameState::LoadLevel(const std::string& aFilePath)
{
	if (myCurrentLevel != "")
	{
		UnloadCurrentLevel();
	}

	return MergeLevel(aFilePath);
}

void GameState::MergeNextLevel()
{
	++myCurrentLevelindex;
	if (myCurrentLevelindex >= myLevelSequence.size())
	{
		return;
	}

	std::string nextLevel = myLevelSequence[myCurrentLevelindex];
	MergeLevel(nextLevel);
}

bool GameState::MergeLevel(const std::string& aFilePath)
{
	if (myCurrentLoadedLevels.count(aFilePath) == 0)
	{
		myCurrentLoadedLevels.insert(aFilePath);
		if (!myAsyncPartialLevel.valid())
		{
			myAsyncPartialLevel = std::async(&GameState::ParseLevelFile, this, aFilePath);
			Message startLoading;
			startLoading.myText = aFilePath;
			startLoading.myMessageType = MessageType::StartLoading;
			SendMessages(startLoading);
			return true;
		}
	}
	return false;
}

bool GameState::MergeQueuedLevelPartially(float aTimeBudget)
{
	PERFORMANCETAG("Level streaming");
	float start = Tools::GetTotalTime();
	size_t count = 0;
	while (myQueuedPartialLevel.myIsComplete)
	{
		if (myQueuedPartialLevel.myIsSuccessfull)
		{
			myQueuedPartialLevel.myIsComplete = false;
			break;
		}
		float now = Tools::GetTotalTime();
		float used = now - start;
		count++;
		if (used > aTimeBudget)
		{
			std::cout << "streaming halted [" + std::to_string(used) + "] Exceded budget [" + std::to_string(aTimeBudget) + "] did [" + std::to_string(count) + "] passes\n";
			return false;
		}
		switch (myQueuedPartialLevel.myStep)
		{
		case 0:
		{
			PERFORMANCETAG("Skybox");
			Skybox* skybox = AssetManager::GetInstance().GetSkybox(myQueuedPartialLevel.mySkyboxPath).InstansiateSkybox();
			if (skybox)
			{
				delete mySkybox;
				myScene->SetSkybox(skybox);
				mySkybox = skybox;
			}
			++myQueuedPartialLevel.myStep;
		}
		break;
		case 1:
		{
			PERFORMANCETAG("Decals");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myDecals.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				auto& package = myQueuedPartialLevel.myDecals[myQueuedPartialLevel.myCounter];
				Decal* decal = myDecalFactory.LoadDecal(package.aFilePath, package.aRotation, package.aTranslation);
				myScene->AddToScene(decal);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 2:
		{
			{
				PERFORMANCETAG("Enemies");
				if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myEnemyObjects.size())
				{
					myQueuedPartialLevel.myCounter = 0;
					++myQueuedPartialLevel.myStep;
				}
				else
				{
					Message enemySpawnMessage;
					enemySpawnMessage.myMessageType = MessageType::SpawnEnemy;
					enemySpawnMessage.myData = &myQueuedPartialLevel.myEnemyObjects[myQueuedPartialLevel.myCounter];
					Publisher::SendMessages(enemySpawnMessage);
					++myQueuedPartialLevel.myCounter;
				}
			}
		}
		break;
		case 3:
		{
			PERFORMANCETAG("Triggers");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myTriggerBuffer.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				Message triggerSpawnMessage;
				triggerSpawnMessage.myMessageType = MessageType::SpawnTriggerBox;
				triggerSpawnMessage.myData = &myQueuedPartialLevel.myTriggerBuffer[myQueuedPartialLevel.myCounter];
				Publisher::SendMessages(triggerSpawnMessage);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 4:
		{
			PERFORMANCETAG("Waypoints");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myMayaPositions.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				myNodePollingStation->AddMayaPos(myQueuedPartialLevel.myMayaPositions[myQueuedPartialLevel.myCounter].first, myQueuedPartialLevel.myMayaPositions[myQueuedPartialLevel.myCounter].second);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 5:
		{
			PERFORMANCETAG("Environmentlight");
			EnvironmentLight* envoLight = myLightLoader->LoadLight(myQueuedPartialLevel.mySkyboxPath.substr(0, myQueuedPartialLevel.mySkyboxPath.length() - 4) + "_light.dds");

			if (envoLight)
			{
				envoLight->myDirection = V3F(-myQueuedPartialLevel.myEnvironmentlight.direction[0], -myQueuedPartialLevel.myEnvironmentlight.direction[1], -myQueuedPartialLevel.myEnvironmentlight.direction[2]);
				envoLight->myColor = V3F(myQueuedPartialLevel.myEnvironmentlight.color[0], myQueuedPartialLevel.myEnvironmentlight.color[1], myQueuedPartialLevel.myEnvironmentlight.color[2]);
				envoLight->myIntensity = myQueuedPartialLevel.myEnvironmentlight.intensity;
				myScene->SetEnvironmentLight(envoLight);
			}
			++myQueuedPartialLevel.myStep;
		}
		break;
		case 6:
		{
			PERFORMANCETAG("Pointlights");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myPointLights.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				Message triggerLightSpawnMessage;
				triggerLightSpawnMessage.myMessageType = MessageType::SpawnPointLight;
				triggerLightSpawnMessage.myData = &myQueuedPartialLevel.myPointLights[myQueuedPartialLevel.myCounter];
				Publisher::SendMessages(triggerLightSpawnMessage);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 7:
		{
			PERFORMANCETAG("NavMesh");
			PathFinder::GetInstance().GenerateFromMesh(myQueuedPartialLevel.myNavMeshPath);
			{

				PERFORMANCETAG("Baking");
				int nrOfTriangles = CAST(int, PathFinder::GetInstance().GetMyPathFinderData()->myNodes.size());
				std::vector<V3F> verts = PathFinder::GetInstance().GetMyPathFinderData()->myVertexCollection;
				std::vector<int> indices;
				for (auto& it : PathFinder::GetInstance().GetMyPathFinderData()->myNodes)
				{
					indices.push_back(CAST(int, it.myCorners[0]));
					indices.push_back(CAST(int, it.myCorners[1]));
					indices.push_back(CAST(int, it.myCorners[2]));
				}
				myGBPhysX->GBCreateNavMesh(CAST(int, verts.size()), verts, nrOfTriangles, indices);
				++myQueuedPartialLevel.myStep;
			}
		}
		break;
		case 8:
		{
			PERFORMANCETAG("CollisionBoxes");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myCollsionBoxes.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				Message spawnGBPhysXStaticMessage;
				spawnGBPhysXStaticMessage.myMessageType = MessageType::SpawnGBPhysXBox;
				spawnGBPhysXStaticMessage.myData = &myQueuedPartialLevel.myCollsionBoxes[myQueuedPartialLevel.myCounter];
				Publisher::SendMessages(spawnGBPhysXStaticMessage);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 9:
		{
			PERFORMANCETAG("Destructables");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myDestrucables.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				Message destructibleSpawnMessage;
				destructibleSpawnMessage.myMessageType = MessageType::SpawnDestructibleObject;
				destructibleSpawnMessage.myData = &myQueuedPartialLevel.myDestrucables[myQueuedPartialLevel.myCounter];
				destructibleSpawnMessage.myBool = false;
				Publisher::SendMessages(destructibleSpawnMessage);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 10:
		{
			PERFORMANCETAG("Spotlights");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.mySpotlights.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				auto& objBuffer = myQueuedPartialLevel.mySpotlights[myQueuedPartialLevel.myCounter];
				SpotLight* l = mySpotlightFactory.LoadSpotlight(objBuffer.aFilePath, objBuffer.aFov, objBuffer.aRange, objBuffer.aIntensity, objBuffer.aRotation, objBuffer.aTranslation);
				myScene->AddToScene(l);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 11:
		{
			PERFORMANCETAG("Interactable objects");
			if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myInteractables.size())
			{
				myQueuedPartialLevel.myCounter = 0;
				++myQueuedPartialLevel.myStep;
			}
			else
			{
				Message spawnInteractableMessage;
				spawnInteractableMessage.myMessageType = MessageType::SpawnInteractable;
				spawnInteractableMessage.myData = &myQueuedPartialLevel.myInteractables[myQueuedPartialLevel.myCounter];
				Publisher::SendMessages(spawnInteractableMessage);
				++myQueuedPartialLevel.myCounter;
			}
		}
		break;
		case 12:
		{
			{
				PERFORMANCETAG("Static objects");
				if (myQueuedPartialLevel.myCounter >= myQueuedPartialLevel.myStaticObjects.size())
				{
					myQueuedPartialLevel.myCounter = 0;
					++myQueuedPartialLevel.myStep;
				}
				else
				{
					Message spawnStaticMessage;
					spawnStaticMessage.myMessageType = MessageType::SpawnStaticObject;
					spawnStaticMessage.myData = &myQueuedPartialLevel.myStaticObjects[myQueuedPartialLevel.myCounter];
					Publisher::SendMessages(spawnStaticMessage);
					++myQueuedPartialLevel.myCounter;
				}
			}
		}
		break;
		default:
		{
			PERFORMANCETAG("Finishing up");
			for (auto& i : *myScene)
			{
				i->SetIsHighlighted(false);
			}
			myCurrentLevel = myQueuedPartialLevel.myLevelFile;

			Message newLevelLoadedMessage;
			newLevelLoadedMessage.myMessageType = MessageType::NewLevelLoaded;
			newLevelLoadedMessage.myIntValue = myQueuedPartialLevel.myWorldAxisSize;
			newLevelLoadedMessage.myIntValue2 = myCurrentLevelindex;
			newLevelLoadedMessage.myData = &myQueuedPartialLevel.myPlayerStart;
			Publisher::SendMessages(newLevelLoadedMessage);
			myGraphManager.ReTriggerTree();

			myLatestCheckpointPos = myQueuedPartialLevel.myPlayerStart;

			myQueuedPartialLevel = LevelParseResult();
			myQueuedPartialLevel.myIsComplete = false;
		}
		break;
		}
	}

	return true;
}


bool ParseSkybox(std::istream& aStream, LevelParseResult& aResult)
{
	std::getline(aStream, aResult.mySkyboxPath);
	if (!aStream)
	{
		SYSERROR("Loading level failed at skybox setup", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseWorldAxis(std::istream& aStream, LevelParseResult& aResult)
{
	aStream >> aResult.myWorldAxisSize;
	if (!aStream)
	{
		SYSERROR("Loading level failed at world axis", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseStaticObjects(std::istream& aStream, LevelParseResult& aResult)
{

	int modelCount = 0;
	aStream >> modelCount;

	StaticObjectInstanceBuffer buffer;
	for (size_t i = 0; i < modelCount; i++)
	{
		if (aStream >> buffer.aFilePath
			>> buffer.position[0] >> buffer.position[1] >> buffer.position[2]
			>> buffer.rotation[0] >> buffer.rotation[1] >> buffer.rotation[2]
			>> buffer.scale[0] >> buffer.scale[1] >> buffer.scale[2])
		{
			aResult.myStaticObjects.push_back(buffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Static objects broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseInteractables(std::istream& aStream, LevelParseResult& aResult)
{
	int modelCount = 0;
	aStream >> modelCount;

	InteractableInstanceBuffer buffer;
	for (size_t i = 0; i < modelCount; i++)
	{
		if (aStream >> buffer.aFilePath >> buffer.interactID
			>> buffer.position[0] >> buffer.position[1] >> buffer.position[2]
			>> buffer.rotation[0] >> buffer.rotation[1] >> buffer.rotation[2]
			>> buffer.scale[0] >> buffer.scale[1] >> buffer.scale[2]
			>> buffer.interactPos[0] >> buffer.interactPos[1] >> buffer.interactPos[2] >> buffer.aImagePath)
		{
			aResult.myInteractables.push_back(buffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Pickup objects broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseDecals(std::istream& aStream, LevelParseResult& aResult)
{
	int decalCount = 0;
	aStream >> decalCount;

	DecalBuffer buffer;
	for (size_t i = 0; i < decalCount; i++)
	{
		if (aStream >> buffer.aFilePath
			>> buffer.aTranslation.x >> buffer.aTranslation.y >> buffer.aTranslation.z
			>> buffer.aRotation.x >> buffer.aRotation.y >> buffer.aRotation.z)
		{
			aResult.myDecals.push_back(buffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Pickup objects broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseEnemies(std::istream& aStream, LevelParseResult& aResult)
{

	int enemyCount = 0;
	aStream >> enemyCount;

	EnemyInstanceBuffer enemyBuffer;

	for (size_t i = 0; i < enemyCount; i++)
	{
		if (aStream >> enemyBuffer.aFilePath
			>> enemyBuffer.enemyID
			>> enemyBuffer.animationID >> enemyBuffer.triggerVolumeID >> enemyBuffer.targetPosID
			>> enemyBuffer.position[0] >> enemyBuffer.position[1] >> enemyBuffer.position[2]
			>> enemyBuffer.rotation[0] >> enemyBuffer.rotation[1] >> enemyBuffer.rotation[2]
			>> enemyBuffer.scale[0] >> enemyBuffer.scale[1] >> enemyBuffer.scale[2])
		{
			aResult.myEnemyObjects.push_back(enemyBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Enemies broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseTriggers(std::istream& aStream, LevelParseResult& aResult)
{
	int triggerCount = 0;
	aStream >> triggerCount;

	TriggerBoxInstanceBuffer triggerBuffer;

	for (size_t i = 0; i < triggerCount; i++)
	{
		if (aStream >> triggerBuffer.aFilePath
			>> triggerBuffer.ID
			>> triggerBuffer.targetID
			>> triggerBuffer.position[0] >> triggerBuffer.position[1] >> triggerBuffer.position[2]
			>> triggerBuffer.rotation[0] >> triggerBuffer.rotation[1] >> triggerBuffer.rotation[2]
			>> triggerBuffer.scale[0] >> triggerBuffer.scale[1] >> triggerBuffer.scale[2] >> triggerBuffer.isAbilityTriggered)
		{
			aResult.myTriggerBuffer.push_back(triggerBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Triggers broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParsePoints(std::istream& aStream, LevelParseResult& aResult)
{
	//Positions
	int posCount = 0;
	aStream >> posCount;
	V3F pos;
	int ID;

	for (size_t i = 0; i < posCount; i++)
	{
		if (aStream >> ID >> pos.x >> pos.y >> pos.z)
		{
			aResult.myMayaPositions.emplace_back(ID, pos);
		}
	}

	if (!aStream)
	{
		SYSERROR("Maya positions broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseEnvironmentLight(std::istream& aStream, LevelParseResult& aResult)
{
	DirLightInstanceBuffer dirLightBuffer;
	dirLightBuffer.color[0] = 1.f;
	dirLightBuffer.color[1] = 1.f;
	dirLightBuffer.color[2] = 1.f;

	dirLightBuffer.direction[0] = 0.3f;
	dirLightBuffer.direction[1] = 0.5f;
	dirLightBuffer.direction[2] = 0.1f;
	dirLightBuffer.intensity = 1.f;

	std::string dirLight = "";
	int bob;
	aStream >> bob;
	aStream >> dirLight;

	if (dirLight == "DirectionalLight" && aStream
		>> dirLightBuffer.direction[0] >> dirLightBuffer.direction[1] >> dirLightBuffer.direction[2]
		>> dirLightBuffer.color[0] >> dirLightBuffer.color[1] >> dirLightBuffer.color[2] >> dirLightBuffer.intensity)
	{
		aResult.myEnvironmentlight = dirLightBuffer;
	}
	if (!aStream)
	{
		SYSERROR("Environmentlight  broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParsePointLights(std::istream& aStream, LevelParseResult& aResult)
{
	int lightCount = 0;
	aStream >> lightCount;
	std::string bob;

	PointLightInstanceBuffer lightBuffer;

	for (size_t i = 0; i < lightCount; i++)
	{
		if (aStream >> bob >> lightBuffer.id
			>> lightBuffer.position[0] >> lightBuffer.position[1] >> lightBuffer.position[2]
			>> lightBuffer.intensity >> lightBuffer.period >> lightBuffer.minVal >> lightBuffer.color[0]
			>> lightBuffer.color[1] >> lightBuffer.color[2] >> lightBuffer.range)
		{
			aResult.myPointLights.push_back(lightBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("point lights broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParsePlayerPos(std::istream& aStream, LevelParseResult& aResult)
{
	int dummyInt;
	int dummyInt2;
	if (!(aStream >> dummyInt >> dummyInt2 >> aResult.myPlayerStart.x >> aResult.myPlayerStart.y >> aResult.myPlayerStart.z))
	{
		SYSERROR("Player spawn position broke level loading", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseNavMesh(std::istream& aStream, LevelParseResult& aResult)
{
	int dummy;
	if (!(aStream >> dummy >> aResult.myNavMeshPath))
	{
		SYSERROR("navMesh broke level loading", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseCollisionBoxes(std::istream& aStream, LevelParseResult& aResult)
{
	int boxCount = 0;
	aStream >> boxCount;

	PhysXBoxInstanceBuffer physXBuffer;

	for (size_t i = 0; i < boxCount; i++)
	{
		if (aStream >> physXBuffer.aFilePath
			>> physXBuffer.position[0] >> physXBuffer.position[1] >> physXBuffer.position[2]
			>> physXBuffer.rotation[0] >> physXBuffer.rotation[1] >> physXBuffer.rotation[2]
			>> physXBuffer.scale[0] >> physXBuffer.scale[1] >> physXBuffer.scale[2])
		{
			aResult.myCollsionBoxes.push_back(physXBuffer);
		}
	}
	if (!aStream)
	{
		SYSERROR("physX boxes broke level loader", aResult.myLevelFile);
		return false;
	}
}

bool ParseDestructables(std::istream& aStream, LevelParseResult& aResult)
{
	int destructiblecount = 0;
	aStream >> destructiblecount;

	DestructibleObjectInstanceBuffer destructibleBuffer;

	for (size_t i = 0; i < destructiblecount; i++)
	{
		if (aStream >> destructibleBuffer.aFilePath
			>> destructibleBuffer.position[0] >> destructibleBuffer.position[1] >> destructibleBuffer.position[2]
			>> destructibleBuffer.rotation[0] >> destructibleBuffer.rotation[1] >> destructibleBuffer.rotation[2]
			>> destructibleBuffer.scale[0] >> destructibleBuffer.scale[1] >> destructibleBuffer.scale[2] >> destructibleBuffer.type)
		{
			aResult.myDestrucables.push_back(destructibleBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("destructibles boxes broke level loader", aResult.myLevelFile);
		return false;
	}
}

bool ParseSpotlights(std::istream& aStream, LevelParseResult& aResult)
{
	int spotLightCount = 0;
	aStream >> spotLightCount;
	SpotLightBuffer spotBuffer;

	for (size_t i = 0; i < spotLightCount; i++)
	{
		if (aStream >> spotBuffer.aFilePath
			>> spotBuffer.aFov
			>> spotBuffer.aRange
			>> spotBuffer.aIntensity
			>> spotBuffer.aTranslation.x >> spotBuffer.aTranslation.y >> spotBuffer.aTranslation.z
			>> spotBuffer.aRotation.x >> spotBuffer.aRotation.y >> spotBuffer.aRotation.z)
		{
			aResult.mySpotlights.push_back(spotBuffer);
		}
	}
	if (!aStream)
	{
		SYSERROR("Spotlights broke level loader", aResult.myLevelFile);
		return false;
	}
}

bool ParseDynamicObjects(std::istream& aStream, LevelParseResult& aResult)
{
	int dynamicObjectCount = 0;
	aStream >> dynamicObjectCount;
	SpotLightBuffer spotBuffer;
	float _;

	for (size_t i = 0; i < dynamicObjectCount; i++)
	{
		if (aStream >> spotBuffer.aFilePath
			>> _ >> _ >> _
			>> _ >> _ >> _
			>> _ >> _ >> _)
		{
			//what is this even?
		}
	}
	if (!aStream)
	{
		SYSERROR("dynamicObjects broke level loader", aResult.myLevelFile);
		return false;
	}
}

LevelParseResult GameState::ParseLevelFile(const std::string& aFilePath)
{
	LevelParseResult result;
	result.myCounter = 0;
	result.myStep = 0;

	std::ifstream levelFile;
	levelFile.open(aFilePath);
	if (!levelFile.good())
	{
		SYSERROR("Could not open/find levelfile", aFilePath);
		return result;
	}

	result.myIsSuccessfull &= ParseSkybox(levelFile, result);
	result.myIsSuccessfull &= ParseWorldAxis(levelFile, result);
	result.myIsSuccessfull &= ParseStaticObjects(levelFile, result);
	result.myIsSuccessfull &= ParseEnemies(levelFile, result);
	result.myIsSuccessfull &= ParsePoints(levelFile, result);
	result.myIsSuccessfull &= ParseTriggers(levelFile, result);
	result.myIsSuccessfull &= ParseEnvironmentLight(levelFile, result);
	result.myIsSuccessfull &= ParsePointLights(levelFile, result);
	result.myIsSuccessfull &= ParsePlayerPos(levelFile, result);
	result.myIsSuccessfull &= ParseNavMesh(levelFile, result);
	result.myIsSuccessfull &= ParseCollisionBoxes(levelFile, result);
	result.myIsSuccessfull &= ParseDestructables(levelFile, result);
	result.myIsSuccessfull &= ParseSpotlights(levelFile, result);
	result.myIsSuccessfull &= ParseDecals(levelFile, result);
	result.myIsSuccessfull &= ParseDynamicObjects(levelFile, result);
	result.myIsSuccessfull &= ParseInteractables(levelFile, result);


	return result;
}

void GameState::UnloadCurrentLevel()
{
	myGameWorld->ClearWorld();
	myScene->RemoveAll();
	//TEMPORARY PHYSX WORLD CLEAN probably to be exchanged for entities cleaning their own physx actor
	myGBPhysX->GBResetScene();
}

void GameState::UnloadLevel(std::string aFilepath)
{
	Message unloadMessage;
	unloadMessage.myText = aFilepath;
	unloadMessage.myMessageType = MessageType::UnloadLevel;
	SendMessages(unloadMessage);
}

void GameState::CreateWorld(WindowHandler* aWindowHandler, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myScene = new Scene();
	myDevice = aFramework->GetDevice();

	Camera* camera = CCameraFactory::CreateCamera(90.f, true);
	camera->SetPosition(V3F(0, 0, -1000.f));
	camera->SetFov(81.f, false);
	camera->SetFov(65.f, true);

	myScene->AddInstance(camera);
	myScene->SetMainCamera(camera);
#ifdef _DEBUG
	DebugTools::myCamera = camera;
	DebugTools::Setup(myScene);
#endif // _DEBUG


	SAFE_DELETE(myNodePollingStation);
	myNodePollingStation = new NodePollingStation();
	CNodeInstance::ourPollingStation = myNodePollingStation;
	myGameWorld->SystemLoad(myModelLoader, mySpriteFactory, myScene, aFramework, aAudioManager, myGBPhysX, aSpriteRenderer, myLightLoader);

	myGameWorld->Init(myModelLoader, mySpriteFactory, myScene, aFramework, camera, myNodePollingStation, &mySpotlightFactory);	//LoadDefaultLevel();
	myNodePollingStation->SetTimerController(&myTimerController);
	myNodePollingStation->SetParticleFactory(myGameWorld->GetParticleFactory());
	myNodePollingStation->SetModelLoader(myModelLoader);
	myNodePollingStation->SetScene(myScene);
	myNodePollingStation->SetSpriteFactory(mySpriteFactory);

#if USEIMGUI
	std::thread thread(std::bind(&GameState::SearchForFiles, this));
	thread.detach();
#endif


	std::ifstream levelSequencefile;
	levelSequencefile.open("Data/Levels/Sequence.levels");
	std::string row;
	while (std::getline(levelSequencefile, row))
	{
		Tools::trim(row);
		myLevelSequence.push_back(row);
	}



	//NextLevel();
}

void GameState::NextLevel()
{
#if DEMOSCENE
	LoadLevel("");
	return;
#endif
	if (myLevelSequence.empty())
	{
		EnvironmentLight* light = myLightLoader->LoadLight("Data/Textures/Skyboxes/skyBox_Level2_light.dds");
		if (light)
		{
			light->myDirection = V3F(1, 1, 1);
			light->myIntensity = 1;
			light->myColor = V3F(1, 1, 1);

			EnvironmentLight* oldLight = myScene->GetEnvironmentLight();
			myScene->SetEnvironmentLight(light);
			SAFE_DELETE(oldLight);
		}

		return;
	}

	++myCurrentLevelindex;
	if (myCurrentLevelindex >= myLevelSequence.size())
	{
		return;
	}

	std::string nextLevel = myLevelSequence[myCurrentLevelindex];
	//myLevelSequence.pop();
	if (!LoadLevel(nextLevel))
	{
		SYSERROR("Could not load level trying to load next level", nextLevel);
		NextLevel();
	}
	Message levelNumberMessage;
	levelNumberMessage.myMessageType = MessageType::CurrentLevel;
	levelNumberMessage.myIntValue = myCurrentLevelindex - 1;
	SendMessages(levelNumberMessage);
}

void GameState::WinGame()
{
	SYSINFO("Won the game!!");
	myIsGameWon = true;
}

void GameState::Activate()
{
	PostMaster::GetInstance()->SendMessages(MessageType::GameActive);
}

void GameState::Deactivate()
{
	PostMaster::GetInstance()->SendMessages(MessageType::GameNotActive);
}

void GameState::Unload()
{
	myGameWorld->ClearWorld(true);
	//mySplines.clear();
	myScene->RemoveAll();
}

void GameState::Update(const float aDeltaTime)
{
	float dt = CLAMP(0, 0.03f, aDeltaTime);
	PERFORMANCETAG("GameState update");
#ifdef _DEBUG
	DebugTools::UpdateGizmo();
#endif // _DEBUG

	if (myQueuedPartialLevel.myIsComplete)
	{
		if (myHasRenderedAtleastOneFrame)
		{
			MergeQueuedLevelPartially(INFINITY);
		}
	}
	else
	{
		if (myAsyncPartialLevel.valid() && myAsyncPartialLevel.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
		{
			myQueuedPartialLevel = myAsyncPartialLevel.get();
			myAsyncPartialLevel = std::future<LevelParseResult>();
		}
	}


	WindowControl::Window("Fourier", [this, aDeltaTime]()
		{

			static std::vector<CompF> original;

			static std::vector<CompF> dataPoints;
			static int precision = 4;
			static float curTime = 0.f;
			static float speedScale = 1.f;
			static float ZoomScale = 10.f;
			curTime += aDeltaTime * speedScale;

			static bool IsDrawing = false;

			if (original.empty() && !IsDrawing)
			{
				CompF top = CompF(0.f, ImagF(0.f));
				CompF left = CompF(-1.f, ImagF(-2.f));
				CompF right = CompF(1.f, ImagF(-2.f));

				for (float i = 0.1; i < 1.f; i += 0.01f)
				{
					original.push_back(LERP(top, left, i));
				}
				for (float i = 0; i < 1.f; i += 0.01f)
				{
					original.push_back(LERP(left, right, i));
				}
				for (float i = 0; i < 0.9f; i += 0.01f)
				{
					original.push_back(LERP(right, top, i));
				}

				CompF center = CompF(0.f, ImagF(2.f));
				float radius = 1.6f;

				CompF cirbeg = CompF::EPow(TAU * (0.1f) - PI / 2) * radius + center;
				CompF tribeg = LERP(right, top, 0.9f);

				for (float i = 0; i < 1.0f; i += 0.01f)
				{
					original.push_back(LERP(tribeg, cirbeg, i));
				}

				for (float i = 0; i < 1.f; i += 0.001f)
				{
					original.push_back(CompF::EPow(TAU * (i * 0.8f + 0.1f) - PI / 2) * radius + center);
				}

				CompF cirend = CompF::EPow(TAU * (0.9f) - PI / 2) * radius + center;
				CompF triend = LERP(top, left, 0.1f);

				for (float i = 0; i < 1.0f; i += 0.01f)
				{
					original.push_back(LERP(cirend, triend, i));
				}
			}





			static std::vector<Math::FourierTransformValue> series = Math::DiscreteFourierTransform(original, precision);

			dataPoints.push_back(Math::DescreteFourierInterpolate(series, curTime / 5.f));

			std::vector<CompF> steps = Math::DescreteFourierInterpolateSteps(series, curTime / 5.f);

			V3F offset = V3F(0, 300, 0);
			V3F xaxis = V3F(200, 0, 0);
			V3F yaxis = V3F(0, 0, 200);

			if (IsDrawing)
			{
				if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
				{
					CommonUtilities::Plane<float> p;
					p.InitWithPointAndNormal(offset, xaxis.Cross(yaxis));
					float trash;
					V3F intersect = DebugTools::LastKnownMouseRay->FindIntersection(p, trash);
					V3F delta = intersect - offset;
					float x = xaxis.Dot(delta) / xaxis.Length() / xaxis.Length();
					float y = yaxis.Dot(delta) / yaxis.Length() / yaxis.Length();
					original.push_back(CompF(x, ImagF(y)));
				}
			}

			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				CommonUtilities::Plane<float> p;
				p.InitWithPointAndNormal(offset, xaxis.Cross(yaxis));
				float trash;
				V3F intersect = DebugTools::LastKnownMouseRay->FindIntersection(p, trash);
				V3F delta = intersect - offset;
				float x = xaxis.Dot(delta) / xaxis.Length() / xaxis.Length();
				float y = yaxis.Dot(delta) / yaxis.Length() / yaxis.Length();

				V3F newXaxis = xaxis * ZoomScale;
				V3F newYaxis = yaxis * ZoomScale;
				V3F newOffset = V3F(0, 50, 0) + offset - x * newXaxis - y * newYaxis;

				DebugDrawer::GetInstance().SetColor(V4F(0, 1, 0, 1));
				DebugDrawer::GetInstance().DrawAnyLines(newOffset, { newXaxis, newYaxis }, reinterpret_cast<float*>(original.data()), original.size());
				DebugDrawer::GetInstance().SetColor(V4F(0.7, 0.7, 1, 1));
				DebugDrawer::GetInstance().DrawAnyLines(newOffset, { newXaxis, newYaxis }, reinterpret_cast<float*>(steps.data()), steps.size());
				DebugDrawer::GetInstance().SetColor(V4F(1, 0, 0, 1));
				DebugDrawer::GetInstance().DrawAnyLines(newOffset, { newXaxis, newYaxis }, reinterpret_cast<float*>(dataPoints.data()), dataPoints.size());

				const float boxSize = 0.2f;

				std::array<V3F, 8> corners
				{
					offset + (x - boxSize) * xaxis + (y - boxSize) * yaxis,
					offset + (x + boxSize) * xaxis + (y - boxSize) * yaxis,
					offset + (x - boxSize) * xaxis + (y + boxSize) * yaxis,
					offset + (x + boxSize) * xaxis + (y + boxSize) * yaxis,

					newOffset + (x - boxSize) * newXaxis + (y - boxSize) * newYaxis,
					newOffset + (x + boxSize) * newXaxis + (y - boxSize) * newYaxis,
					newOffset + (x - boxSize) * newXaxis + (y + boxSize) * newYaxis,
					newOffset + (x + boxSize) * newXaxis + (y + boxSize) * newYaxis
				};

				DebugDrawer::GetInstance().SetColor(V4F(0.7, 1, 0.4, 1));
				DebugDrawer::GetInstance().DrawLine(corners[0], corners[1]);
				DebugDrawer::GetInstance().DrawLine(corners[0], corners[2]);
				DebugDrawer::GetInstance().DrawLine(corners[1], corners[3]);
				DebugDrawer::GetInstance().DrawLine(corners[2], corners[3]);

				DebugDrawer::GetInstance().DrawLine(corners[4], corners[5]);


				DebugDrawer::GetInstance().DrawLine(corners[0], corners[4]);
				DebugDrawer::GetInstance().DrawLine(corners[1], corners[5]);
				DebugDrawer::GetInstance().DrawLine(corners[2], corners[6]);
				DebugDrawer::GetInstance().DrawLine(corners[3], corners[7]);

			}


			DebugDrawer::GetInstance().SetColor(V4F(0, 1, 0, 1));
			DebugDrawer::GetInstance().DrawAnyLines(offset, { xaxis, yaxis }, reinterpret_cast<float*>(original.data()), original.size());
			DebugDrawer::GetInstance().SetColor(V4F(0.7, 0.7, 1, 1));
			DebugDrawer::GetInstance().DrawAnyLines(offset, { xaxis, yaxis }, reinterpret_cast<float*>(steps.data()), steps.size());
			DebugDrawer::GetInstance().SetColor(V4F(1, 0, 0, 1));
			DebugDrawer::GetInstance().DrawAnyLines(offset, { xaxis, yaxis }, reinterpret_cast<float*>(dataPoints.data()), dataPoints.size());

			float shouldReset = false;
			if (ImGui::Button("Reset"))
			{
				shouldReset = true;
			}
			if (ImGui::InputInt("Precision", &precision))
			{
				shouldReset = true;
			}
			ImGui::InputFloat("Speed", &speedScale, 0.01f, 0.03f, 5);
			ImGui::InputFloat("ZoomScale", &ZoomScale, 1.f, 2.0f, 2);

			if (IsDrawing)
			{
				if (ImGui::Button("Finish Drawing"))
				{
					IsDrawing = false;
					shouldReset = true;
				}
			}
			else
			{
				if (ImGui::Button("Draw Shape"))
				{
					original.clear();
					IsDrawing = true;
				}
			}

			if (shouldReset)
			{
				dataPoints.clear();
				series = Math::DiscreteFourierTransform(original, precision);
				curTime = 0;
			}
		});
#if 0
	WindowControl::Window("Mandelbrot", [this]()
		{
			static size_t resolution = 64;

			static double scale = 1.f;
			static CommonUtilities::Vector2<double> center = CommonUtilities::Vector2<double>(0, 0);
			static V2F lastMouse = V2F(0, 0);
			static size_t currentUpdatePosition = 0;
			static size_t depth = 5;

			static ModelInstance* instance;
			static Model* model;
			static ID3D11Texture2D* texture;
			static Texture* wrappedTexture;

			static std::vector<float> data;
			static std::vector<size_t> indirectionMap;

			auto Evaluate = [](CompD aValue, int aDepth) -> int
			{
				CompD Z = CompD(0.f, ImagD(0.f));
				if (aValue.Abs() > 0.25f)
				{
					for (size_t i = 0; i < aDepth; i++)
					{
						Z = Z * Z + aValue;
						if (Z.Abs() > 2.f)
						{
							return i;
						}
					}
				}
				return -1;
			};

			auto UpdatePixel = [&](size_t pixel) -> void
			{
				double y = -(double(pixel / resolution) / double(resolution) - 0.5) * 2.0;
				double x = -(double(pixel % resolution) / double(resolution) - 0.5) * 2.0;
				data[pixel] = Evaluate(CompD(x * scale + center.x, ImagD(y * scale + center.y)), depth);
			};

			auto SetResolution = [&](size_t aResolution) -> void
			{
				std::vector<float> oldData = data;


				data.resize(aResolution * aResolution);

				for (size_t i = 0; i < data.size(); i++)
				{
					double x = double(i % aResolution) / double(aResolution);
					double y = double(i / aResolution) / double(aResolution);

					data[i] = oldData[size_t(x * resolution) + size_t(y * resolution) * resolution];
				}


				indirectionMap.resize(aResolution * aResolution);
				for (size_t i = 0; i < aResolution * aResolution; i++)
				{
					indirectionMap[i] = i;
				}
				auto rng = std::default_random_engine{};
				std::shuffle(std::begin(indirectionMap), std::end(indirectionMap), rng);

				SAFE_RELEASE(texture);

				D3D11_TEXTURE2D_DESC desc;
				WIPE(desc);
				desc.Height = aResolution;
				desc.Width = aResolution;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.MiscFlags = 0;
				desc.Format = DXGI_FORMAT_R32_FLOAT;
				desc.ArraySize = 1;
				desc.MipLevels = 1;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;

				D3D11_SUBRESOURCE_DATA sysData;
				sysData.pSysMem = data.data();
				sysData.SysMemPitch = aResolution * sizeof(float);
				sysData.SysMemSlicePitch = aResolution * aResolution * sizeof(float);

				HRESULT result = myDevice->CreateTexture2D(&desc, &sysData, &texture);
				if (FAILED(result))
				{
					LOGERROR("Failed to create manndelbrot texture");
				}


				ID3D11ShaderResourceView* view;
				{
					HRESULT result = myDevice->CreateShaderResourceView(texture, nullptr, &view);
					if (FAILED(result))
					{
						LOGERROR("Failed to create manndelbrot rsview");
					}
				}
				Texture* old = wrappedTexture;
				wrappedTexture = new Texture(view); // wont update refcounter as its not supposed to be allowed to create these manually

				model->GetModelData()->myTextures[0] = wrappedTexture;
				model->GetModelData()->myTextures[1] = wrappedTexture;
				model->GetModelData()->myTextures[2] = wrappedTexture;

				resolution = aResolution;
				if (old)
				{
					old->Release();
				}
			};

			auto Reset = [&]() -> void
			{
				for (size_t basey = 0; basey < resolution; basey++)
				{
					for (size_t basex = 0; basex < resolution; basex++)
					{
						UpdatePixel(basex + basey * resolution);
					}
				}
			};

#pragma region Setup
			if (!wrappedTexture)
			{
				data.resize(resolution * resolution);

				indirectionMap.resize(resolution * resolution);
				//Setup
				{
					Model::CModelData modelData;

					std::vector<char> vsBlob;
					modelData.myPixelShader = GetPixelShader(myDevice, "Data/Shaders/Custom/MandelbrotTranslator.hlsl");
					modelData.myVertexShader = GetVertexShader(myDevice, "Data/Shaders/Custom/TextureAroundSpawn.hlsl", vsBlob);

					modelData.myForceForward = true;

					modelData.myInputLayout = nullptr;
					modelData.myStride = 0;
					modelData.myIndexBufferFormat = DXGI_FORMAT_R8_UINT;



					modelData.myFilePath = "Mandelbrot";
					modelData.myIsEffect = false;
					modelData.myOffset = 0;
					modelData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					modelData.myshaderTypeFlags = 0;
					modelData.myTextures[0] = nullptr;
					modelData.myTextures[1] = nullptr;
					modelData.myTextures[2] = nullptr;


					model = new Model();
					model->Init(modelData, nullptr, "mandelbrottranslator", "Texturearoundcenter");

					{
						ID3D11Buffer* indexbuffer = nullptr;
						{
							unsigned char indexbufferdata[] =
							{
								0,1,2,
								3,4,5
							};
							D3D11_BUFFER_DESC desc;
							desc.ByteWidth = 6 * sizeof(UINT);
							desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
							desc.CPUAccessFlags = 0;
							desc.MiscFlags = 0;
							desc.StructureByteStride = sizeof(UINT);
							desc.Usage = D3D11_USAGE_IMMUTABLE;

							D3D11_SUBRESOURCE_DATA data;
							data.pSysMem = indexbufferdata;
							data.SysMemPitch = 0;
							data.SysMemSlicePitch = 0;


							HRESULT result = myDevice->CreateBuffer(&desc, &data, &indexbuffer);
							if (FAILED(result))
							{
								LOGERROR("Failed to create mandelbrot indexbuffer");
							}
						}

						Model::LodLevel* level = new Model::LodLevel();
						level->myIndexBuffer = indexbuffer;
						level->myNumberOfIndexes = 6;
						level->myVertexBuffer = nullptr;
						level->myVertexBufferCount = 0;
						model->ApplyLodLevel(level, 0);
					}
					instance = new ModelInstance(model);
					myScene->AddToScene(instance);

				}
				SetResolution(resolution);
			}
#pragma endregion

			auto FindMouse = [&]() -> V2F
			{
				CommonUtilities::Plane<float> p;
				p.InitWithPointAndNormal(V3F(0, 187, 0), V3F(0, 1, 0));
				float trash;
				V3F intersect = DebugTools::LastKnownMouseRay->FindIntersection(p, trash);

				return V2F(intersect.x / 1400.f, intersect.z / 1400.f);
			};

			bool ShouldUpdate = false;
			if (ImGui::Button("Render All"))
			{
				ShouldUpdate = true;
			}

			Tools::ZoomableImGuiImage(wrappedTexture, ImVec2(200, 200));

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				lastMouse = FindMouse();
				SetResolution(256);
				currentUpdatePosition = 0;
			}

			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				V2F now = FindMouse();
				CommonUtilities::Vector2<double> delta = CommonUtilities::Vector2<double>(lastMouse.x, lastMouse.y) - CommonUtilities::Vector2<double>(now.x, now.y);
				lastMouse = FindMouse();
				center += delta * scale;
				CommonUtilities::Vector2<double> pixelTranslation = delta * double(resolution);

				auto oldData = data;

				for (size_t i = 0; i < data.size(); i++)
				{
					double x = CLAMP(0, 1, double(i % resolution) / double(resolution) - delta.x / 2);
					double y = CLAMP(0, 1, double(i / resolution) / double(resolution) - delta.y / 2);

					data[i] = oldData[CLAMP(0, resolution - 1, size_t(x * resolution)) + CLAMP(0, resolution - 1, size_t(y * resolution)) * resolution];
				}
			}

			static bool exporting = false;
			if (!exporting)
			{
				ImGui::ProgressBar(float(currentUpdatePosition) / float(resolution * resolution));
				ImGui::Text("Resolution: " PFSIZET "\t", resolution);
				ImGui::SameLine();
				if (ImGui::Button("<") && resolution > 0x8) { SetResolution(resolution / 2); }
				ImGui::SameLine();
				if (ImGui::Button(">") && resolution < 0x10000) { SetResolution(resolution * 2); }

				ImGui::PushID("depth");
				ImGui::Text("Depth: " PFSIZET "\t", depth);
				ImGui::SameLine();
				if (ImGui::Button("|<")) { depth = 5; }
				ImGui::SameLine();
				if (ImGui::Button("<") && depth > 0x5) { --depth; }
				ImGui::SameLine();
				if (ImGui::Button(">")) { ++depth; }
				ImGui::PopID();
			}

			float delta = ImGui::GetIO().MouseWheel;
			if (delta != 0.f)
			{
				scale *= pow(1.1f, delta);
				SetResolution(256);
				currentUpdatePosition = 0;
			}


			static char exportName[256] = "mandlebrot.dds";
			static size_t exportreso;
			static size_t exportProgress;

			if (ImGui::Button("Export"))
			{
				exportreso = resolution;
				ImGui::OpenPopup("Exporter");
			}

			if (ImGui::BeginPopup("Exporter"))
			{
				if (!exporting)
				{
					ImGui::InputText("Filename", exportName, 255);
					ImGui::Text("Resolution: " PFSIZET "\t", exportreso);
					ImGui::SameLine();
					if (ImGui::Button("<") && exportreso > 0x8) { exportreso /= 2; }
					ImGui::SameLine();
					if (ImGui::Button(">") && exportreso < 0x10000) { exportreso *= 2; }

					ImGui::Text("Depth: " PFSIZET "\t", depth);
					ImGui::SameLine();
					if (ImGui::Button("|<")) { depth = 5; }
					ImGui::SameLine();
					if (ImGui::Button("<") && depth > 0x5) { --depth; }
					ImGui::SameLine();
					if (ImGui::Button(">")) { ++depth; }

					if (ImGui::Button("Accept"))
					{
						SetResolution(exportreso);
						exporting = true;
						exportProgress = 0;
					}
				}
				else
				{
					ImGui::Text("Exporting");
					ImGui::ProgressBar(float(exportProgress) / float(resolution * resolution));

					float startTime = Tools::GetTotalTime();
					while (Tools::GetTotalTime() - startTime < 0.02f)
					{
						if (exportProgress >= resolution * resolution)
						{
							V4F* drawn = new V4F[resolution * resolution];

							for (size_t i = 0; i < resolution * resolution; i++)
							{
								float source = data[i];
								V4F colour;

								if (source < 0.0)
								{
									colour = V4F(0, 0, 0, 1);
								}
								else
								{
									float fr = fmodf(source / 15.0, 1.f);
									float ler = abs((fr - 0.5) * 2);
									colour = LERP(V4F(1, 0, 1, 1), V4F(1, 1, 0, 1), ler);
								}

								drawn[i] = colour;
							}

							ID3D11Texture2D* tex;
							D3D11_TEXTURE2D_DESC desc;
							WIPE(desc);
							desc.Height = resolution;
							desc.Width = resolution;
							desc.CPUAccessFlags = 0;
							desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
							desc.Usage = D3D11_USAGE_IMMUTABLE;
							desc.MiscFlags = 0;
							desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
							desc.ArraySize = 1;
							desc.MipLevels = 1;
							desc.SampleDesc.Count = 1;
							desc.SampleDesc.Quality = 0;

							D3D11_SUBRESOURCE_DATA sysData;
							sysData.pSysMem = drawn;
							sysData.SysMemPitch = resolution * sizeof(V4F);
							sysData.SysMemSlicePitch = resolution * resolution * sizeof(V4F);

							HRESULT result = myDevice->CreateTexture2D(&desc, &sysData, &tex);
							if (FAILED(result))
							{
								LOGERROR("Failed to create manndelbrot texture");
							}

							delete drawn;

							//Save DDS
							std::string s(exportName);
							DirectX::SaveDDSTextureToFile(myContext, tex, std::wstring(s.begin(), s.end()).c_str());

							tex->Release();
							exporting = false;
							break;
						}
						UpdatePixel(exportProgress);

						++exportProgress;
					}
				}
				ImGui::EndPopup();
			}

			if (!exporting)
			{
				if (ShouldUpdate)
				{
					Reset();
				}
				else
				{
					float startTime = Tools::GetTotalTime();
					while (Tools::GetTotalTime() - startTime < 0.02f)
					{
						if (currentUpdatePosition >= resolution * resolution)
						{
							currentUpdatePosition = 0;
							if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
							{
								++depth;
								if (depth > 50 && resolution < 0x10000)
								{
									SetResolution(resolution * 2);
								}
							}
						}
						UpdatePixel(indirectionMap[currentUpdatePosition]);
						++currentUpdatePosition;
					}
				}
			}

			D3D11_MAPPED_SUBRESOURCE sub;
			myContext->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
			memcpy(sub.pData, data.data(), resolution * resolution * sizeof(float));
			myContext->Unmap(texture, 0);
		});
#endif

	WindowControl::Window("ImageSimplyfier", [this]()
		{
			static Texture* Source = nullptr;
			static Texture* Target = nullptr;
			static Texture* Exported = nullptr;
			static size_t imageSize[2];
			static int targetSize[2];
			static float noise = 0.5f;

#pragma push(COLOUR)
#define COLOUR(r,g,b) V4F(r/255.f,g/255.f,b/255.f,1)
			static std::vector<V4F> colorSet =
			{
				COLOUR(159,50,48), //red
				COLOUR(215,125,29), //Orange
				COLOUR(222,188,50), //yellow

				COLOUR(216,148,176), //Pink
				COLOUR(174,79,180), //magenta
				COLOUR(124,52,172), //purple
				COLOUR(66,69,162), //blue
				COLOUR(35,138,152), //cyan
				COLOUR(71,173,208), //light blue
				COLOUR(121,182,40), //lime
				COLOUR(92,112,43), //green

				COLOUR(24,25,31), //black
				COLOUR(75,79,84), //dark gray
				COLOUR(148,148,145), //light gray
				COLOUR(217,218,223), //white

				COLOUR(118,79,51), //brown
			};

			static std::vector<const char*> colorNames =
			{
				"Red",
				"Orange",
				"Yellow",

				"Pink",
				"Magenta",
				"Purple",
				"Blue",
				"Cyan",
				"Light Blue",
				"Lime",
				"Green",

				"Black",
				"Gray",
				"Light Gray",
				"White",

				"Brown"
			};
#pragma pop(COLOUR)
			static int* exportedData = nullptr;

			if (ImGui::Button("SelectImage"))
			{
				ImGui::OpenPopup("ImageSelector");
			}

			if (Source)
			{
				Tools::ZoomableImGuiImage(Source->operator ID3D11ShaderResourceView * (), ImVec2(200, 200));
				ImGui::Text("Width: " PFSIZET, imageSize[0]);
				ImGui::Text("Height: " PFSIZET, imageSize[1]);
				ImGui::Separator();
				for (size_t i = 0; i < colorSet.size(); i++)
				{
					ImGui::PushID(i);
					ImGui::ColorEdit4("", &colorSet[i].x);
					ImGui::PopID();
				}
				ImGui::Separator();
				ImGui::InputInt2("Target dimensions", targetSize);
				if (targetSize[0] < 1)
				{
					targetSize[0] = 1;
				}
				if (targetSize[1] < 1)
				{
					targetSize[1] = 1;
				}
				ImGui::InputFloat("Noise", &noise);

				if (!IsErrorTexture(Source) && ImGui::Button("Generate"))
				{


					SAFE_DELETE(exportedData);
					SAFE_RELEASE(Target);


					std::vector<V4F> ExtractedData;

					size_t targetDataCount = targetSize[0] * targetSize[1];

					ExtractedData.resize(targetDataCount);

					ID3D11Texture2D* tex;
					ID3D11Resource* res;
					Source->operator ID3D11ShaderResourceView* ()->GetResource(&res);

					{
#pragma region directx messystuff
						ID3D11RenderTargetView* rtview = nullptr;
						ID3D11ShaderResourceView* srview = nullptr;
						ID3D11Texture2D* rttex;
						ID3D11Texture2D* transfertex;

						{
							D3D11_TEXTURE2D_DESC desc;
							WIPE(desc);
							desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
							desc.ArraySize = 1;
							desc.SampleDesc.Count = 1;
							desc.SampleDesc.Quality = 0;
							desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
							desc.CPUAccessFlags = 0;
							desc.MipLevels = 1;
							desc.MiscFlags = 0;
							desc.Usage = D3D11_USAGE_DEFAULT;

							desc.Width = targetSize[0];
							desc.Height = targetSize[1];

							myDevice->CreateTexture2D(&desc, nullptr, &rttex);
							myDevice->CreateRenderTargetView(rttex, nullptr, &rtview);
							myDevice->CreateShaderResourceView(rttex, nullptr, &srview);
						}

						{
							D3D11_TEXTURE2D_DESC desc;
							WIPE(desc);
							desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
							desc.ArraySize = 1;
							desc.SampleDesc.Count = 1;
							desc.SampleDesc.Quality = 0;
							desc.BindFlags = 0;
							desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
							desc.MipLevels = 1;
							desc.MiscFlags = 0;
							desc.Usage = D3D11_USAGE_STAGING;

							desc.Width = targetSize[0];
							desc.Height = targetSize[1];

							myDevice->CreateTexture2D(&desc, nullptr, &transfertex);
						}

						Target = new Texture(srview);

						FullscreenRenderer renderer;
						renderer.Init(myFramework);

						myContext->OMSetRenderTargets(1, &rtview, nullptr);
						D3D11_VIEWPORT port;
						port.TopLeftX = 0;
						port.TopLeftY = 0;
						port.Width = targetSize[0];
						port.Height = targetSize[1];
						port.MaxDepth = 1.f;
						port.MinDepth = 0.f;

						myContext->RSSetViewports(1, &port);
						myContext->PSSetShaderResources(0, 1, *Source);
						renderer.Render(FullscreenRenderer::Shader::COPY);


						myFramework->EndFrame();
						myContext->CopyResource(transfertex, rttex);
						myFramework->EndFrame();

						D3D11_MAPPED_SUBRESOURCE mapped;
						myContext->Map(transfertex, 0, D3D11_MAP_READ, 0, &mapped);
						for (size_t i = 0; i < targetSize[1]; i++)
						{
							memcpy(ExtractedData.data() + i * targetSize[0], reinterpret_cast<char*>(mapped.pData) + i * mapped.RowPitch, targetSize[0] * sizeof(V4F));
						}
						myContext->Unmap(transfertex, 0);
						transfertex->Release();
						rttex->Release();
						rtview->Release();

						Target = new Texture(srview);
#pragma endregion
					}

					//populate data


					exportedData = new int[targetDataCount];

					std::vector<float> weights;
					weights.resize(colorSet.size());

					for (size_t i = 0; i < targetDataCount; i++)
					{
						V4F inp = ExtractedData[i];
						int res = 0;
						float closest = 1.f;
						for (size_t c = 0; c < colorSet.size(); c++)
						{
							weights[c] = 1.f / colorSet[c].Distance(inp) + Tools::RandomNormalized() * noise;
							if (closest < weights[c])
							{
								closest = weights[c];
								res = c;
							}
						}
						exportedData[i] = res;
					}

					ID3D11Texture2D* exportedtex;
					ID3D11ShaderResourceView* exportedView;
					{
						D3D11_TEXTURE2D_DESC desc;
						WIPE(desc);
						desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
						desc.ArraySize = 1;
						desc.SampleDesc.Count = 1;
						desc.SampleDesc.Quality = 0;
						desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
						desc.CPUAccessFlags = 0;
						desc.MipLevels = 1;
						desc.MiscFlags = 0;
						desc.Usage = D3D11_USAGE_IMMUTABLE;

						desc.Width = targetSize[0];
						desc.Height = targetSize[1];

						V4F* translatedData = new V4F[targetDataCount];
						for (size_t i = 0; i < targetDataCount; i++)
						{
							translatedData[i] = colorSet[exportedData[i]];
						}

						D3D11_SUBRESOURCE_DATA data;
						data.pSysMem = translatedData;
						data.SysMemPitch = targetSize[0] * sizeof(V4F);
						data.SysMemSlicePitch = targetDataCount * sizeof(V4F);

						myDevice->CreateTexture2D(&desc, &data, &exportedtex);
						myDevice->CreateShaderResourceView(exportedtex, nullptr, &exportedView);
						exportedtex->Release();
						Exported = new Texture(exportedView);
						delete[] translatedData;
					}
				}
			}

			float scale = 1;
			ImGui::InputFloat("Scale", &scale);
			if (Target)
			{
				ImGui::Separator();
				ImGui::Text("intermediate Scaled x" PFFLOAT, scale);
				Tools::ZoomableImGuiImage(Target->operator ID3D11ShaderResourceView * (), ImVec2(targetSize[0] * scale, targetSize[1] * scale));
				ImGui::Separator();
			}
			if (Exported)
			{
				ImGui::Separator();
				ImGui::Text("Exported Scaled x" PFFLOAT, scale);
				Tools::ZoomableImGuiImage(Exported->operator ID3D11ShaderResourceView * (), ImVec2(targetSize[0] * scale, targetSize[1] * scale));
				ImGui::Separator();

				if (ImGui::Button("Save"))
				{
					ImGui::OpenPopup("Filesaver");
				}

				if (ImGui::Button("Export"))
				{
					ImGui::OpenPopup("Fileexporter");
				}
			}

			if (ImGui::BeginPopup("Filesaver"))
			{
				static char filename[256] = "SimplyfiedImage.dds";
				ImGui::InputText("Filepath", filename, 256);
				if (ImGui::Button("Save"))
				{
					std::string sfilename = filename;
					ID3D11Resource* res;
					Exported->operator ID3D11ShaderResourceView* ()->GetResource(&res);
					DirectX::SaveDDSTextureToFile(myContext, res, std::wstring(sfilename.begin(), sfilename.end()).c_str());
					res->Release();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("Fileexporter"))
			{
				static char filename[256] = "keySequence.txt";
				ImGui::InputText("Filepath", filename, 256);
				if (ImGui::Button("export"))
				{
					std::vector<int> orderedSequence;
					orderedSequence.reserve(targetSize[0] * targetSize[1]);

					for (int x = 0; x < targetSize[0]; x++)
					{
						for (int y = targetSize[1] - 1; y >= 0; y--)
						{
							orderedSequence.push_back(exportedData[x + y * targetSize[0]]);
						}
					}

					std::vector<int>  coloursslot;
					coloursslot.resize(colorSet.size());
					for (auto& i : coloursslot)
					{
						i = -1;
					}

					int fillingSlot = 0;
					for (size_t i = 0; i < orderedSequence.size(); i++)
					{
						int c = orderedSequence[i];
						if (coloursslot[c] == -1)
						{
							coloursslot[c] = fillingSlot++;
							if (fillingSlot >= 9)
							{
								break;
							}
						}
					}

					std::ofstream outfile(filename);

					outfile << "#Make sure you have ";
					for (size_t i = 0; i < 9; i++)
					{
						outfile << "[" << (i + 1) << ": ";
						for (size_t c = 0; c < coloursslot.size(); c++)
						{
							if (coloursslot[c] == i)
							{
								outfile << colorNames[c];
								break;
							}
						}
						outfile << "] ";
					}
					outfile << "in that order on your hotbar" << std::endl;

					int height = 0;
					for (size_t i = 0; i < orderedSequence.size(); i++)
					{
						int c = orderedSequence[i];
						if (coloursslot[c] == -1)
						{
							std::array<bool, 9> hasSeen;
							for (auto& i : hasSeen)
							{
								i = false;
							}

							for (size_t seekahead = i; seekahead < orderedSequence.size(); seekahead++)
							{
								int c = orderedSequence[seekahead];
								int slot = coloursslot[c];
								if (slot != -1)
								{
									if (!hasSeen[slot])
									{
										hasSeen[slot] = true;
										int unseen = 0;
										for (auto& b : hasSeen)
										{
											if (!b)
											{
												++unseen;
											}
										}
										if (unseen <= 1)
										{
											break;
										}
									}
								}
							}
							for (size_t replaceSlot = 0; replaceSlot < 9; replaceSlot++)
							{
								if (!hasSeen[replaceSlot])
								{
									for (size_t col = 0; col < coloursslot.size(); col++)
									{
										if (coloursslot[col] == replaceSlot)
										{
											outfile << std::endl << "#Replace slot [" << (replaceSlot + 1) << ":" << colorNames[col] << "] with ";
											coloursslot[col] = -1;
											break;
										}
									}
									outfile << "[" << (replaceSlot + 1) << ": " << colorNames[c] << "]" << std::endl;
									coloursslot[c] = replaceSlot;
								}
							}
						}

						if (height == targetSize[1])
						{
							outfile << std::endl << "#Move one step to the right" << std::endl;
							height = 0;
						}
						height++;
						outfile << (coloursslot[c] + 1);
					}


					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}



			if (ImGui::BeginPopup("ImageSelector"))
			{
				static char Filepath[256] = "sourceImage.dds";
				ImGui::InputText("FilePath", Filepath, 256);
				if (ImGui::Button("Load"))
				{
					if (Source && !IsErrorTexture(Source))
					{
						Source->Release();
					}

					Source = LoadTexture(myDevice, Filepath);

					ID3D11Resource* res;
					ID3D11ShaderResourceView* view;

					Source->operator ID3D11ShaderResourceView* ()->GetResource(&res);


					ID3D11Texture2D* tex;

					res->QueryInterface<ID3D11Texture2D>(&tex);
					if (tex)
					{
						D3D11_TEXTURE2D_DESC desc;
						tex->GetDesc(&desc);
						imageSize[0] = desc.Width;
						imageSize[1] = desc.Height;
						tex->Release();
					}
					else
					{
						imageSize[0] = 1;
						imageSize[1] = 1;
					}
					res->Release();

				}
				if (Source)
				{
					Tools::ZoomableImGuiImage(Source->operator ID3D11ShaderResourceView * (), ImVec2(200, 200));
				}


				ImGui::EndPopup();
			}


		});
#if DONETWORK
	myClient.Flush();
#if USEIMGUI
	WindowControl::Window("Network", [this]() {
		if (ImGui::Button("Test DeathMarker"))
		{
			Message mess;
			mess.myMessageType = MessageType::AnnounceDeathMarker;
			V3F pos = myGameWorld->GetPlayerPosition();
			mess.myData = &pos;
			SendMessages(mess);
		}
		myClient.ImGui();
		});
#endif // USEIMGUI
#endif

#if ENABLENUMPADLEVELSELECT
	for (int i = 0; i < 10; i++)
	{
		if (GetAsyncKeyState(VK_NUMPAD0 + i) && GetActiveWindow() == GetFocus())
		{
#if USEIMGUI
			if (!ImGui::GetIO().WantCaptureKeyboard)
			{
#endif // USEIMGUI
				myCurrentLevelindex = i - 1;
				NextLevel();
#if USEIMGUI
			}
#endif // USEIMGUI
		}
	}
	{
		static bool pressed = false;
		if (GetAsyncKeyState('N') && GetActiveWindow() == GetForegroundWindow())
		{
			if (!pressed)
			{
				pressed = true;
#if USEIMGUI
				ImGui::OpenPopup("MergeSelector");
#else
				MergeNextLevel();
#endif
			}
		}
		else
		{
			pressed = false;
		}
		if (ImGui::BeginPopup("MergeSelector"))
		{
#ifdef _DEBUG
			ImGui::Text("Select a level to load");
			ImGui::Separator();
			if (myQueuedPartialLevel.myIsComplete)
			{
				ImGui::Text("Currently busy streaming");
			}
			else
			{
				for (auto& i : (*DebugTools::FileList)[".lvl"])
				{
					ImGui::Text(myCurrentLoadedLevels.count(i) != 0 ? "[Loaded  ]" : "[Unloaded]");
					ImGui::SameLine();
					if (ImGui::Selectable(i.c_str()))
					{
						MergeLevel(i);
					}
				}
			}
#else
			ImGui::Text("This feature is only available in debug");
#endif // _DEBUG
			ImGui::EndPopup();
		}

	}
	{
		static bool pressed = false;
		if (GetAsyncKeyState(VK_DELETE) && GetActiveWindow() == GetForegroundWindow())
		{
			if (!pressed)
			{
				pressed = true;
#if USEIMGUI
				ImGui::OpenPopup("UnloadSelector");
#else
				static int counter = 0;
				UnloadLevel(myLevelSequence[counter++]);
#endif
			}
		}
		else
		{
			pressed = false;
		}
		if (ImGui::BeginPopup("UnloadSelector"))
		{
#ifdef _DEBUG
			ImGui::Text("Select a level to unload");
			ImGui::Separator();
			if (myQueuedPartialLevel.myIsComplete)
			{
				ImGui::Text("Currently busy streaming");
			}
			else
			{
				for (auto& i : (*DebugTools::FileList)[".lvl"])
				{
					ImGui::Text(myCurrentLoadedLevels.count(i) != 0 ? "[Loaded  ]" : "[Unloaded]");
					ImGui::SameLine();
					if (ImGui::Selectable(i.c_str()))
					{
						UnloadLevel(i);
					}
				}
			}
#else
			ImGui::Text("This feature is only available in debug");
#endif // _DEBUG
			ImGui::EndPopup();
		}
	}
#endif // 0
	myTimerController.CheckTimers();

#if true // Decal/spotlight testing
	static bool pressedF = false;
	if (GetAsyncKeyState('F'))
	{
		if (!pressedF)
		{
			Decal* l = myDecalFactory.LoadDecal("Data/Decals/hastfan.json", myScene->GetMainCamera());
			myScene->AddToScene(l);
			pressedF = true;
		}
	}
	else
	{
		pressedF = false;
	}

#endif // true // Decal/spotlight testing


#if USEFILEWATHCER
	{
		PERFORMANCETAG("FileWatchers");
		myWatcher.FlushChanges();
		FlushShaderChanges();
		Texture::Flush();
	}
#endif // USEFILEWATCHER
#if DEMOSCENE
	for (auto& i : myLights)
	{
		i->position.RotateY(aDeltaTime);
		if (i->myModel)
		{
			i->myModel->SetPosition(V4F(i->position, 1));
		}
	}
#endif // DEMOSCENE


#if USEIMGUI
	RunImGui(dt); // calls mygameworld.Update inside so gameworld gets a tab inside the window;
#if !BAKEVALUES
	{
		PERFORMANCETAG("Metric editor")
			MetricHandler::ImGuiValueEditor();
	}
#endif
#else
	myGameWorld->Update(*myInputManager->GetInputHandler(), aDeltaTime);
#endif
	// PHYSICS UPDATE ---------------------------------------
	{
		PERFORMANCETAG("PhysX");
		if (myGBPhysX->GetGBPhysXActive())
		{
			//if (aDeltaTime > 0.016f)
			//{
			//	myGBPhysX->GBStepPhysics(0.016f);
			//}
			//else
			//{
			myGBPhysX->GBStepPhysics(aDeltaTime);
			//}
		}
	}

	{
		PERFORMANCETAG("Scripts");
		myGraphManager.ReTriggerUpdateringTrees();
	}
	{
		PERFORMANCETAG("Graphical updates");
		myScene->Update(aDeltaTime);
	}

	if (myIsGameWon)
	{
		myGameWorld->ClearWorld(true);

		Message message;
		message.myMessageType = MessageType::PopState;
		message.myBool = true;
		Publisher::SendMessages(message);

		Message message2;
		message2.myMessageType = MessageType::PushState;
		VideoState* video = new VideoState();
		if (video->Init(myModelLoader, mySpriteFactory, "Data\\Cinematics\\Credits.mp4", false, myContext) == false)
		{
			//TODO: PROPER DELETE OF DATA
			delete video;
			return;
		}
		video->SetMain(true);
		message2.myState = video;
		message2.myData = video;
		Publisher::SendMessages(message2);
	}
}

#if USEIMGUI
void GameState::SearchForFiles()
{
	using namespace std::experimental::filesystem;
	myFoundFiles.clear();
	recursive_directory_iterator it = recursive_directory_iterator(canonical(""));
	size_t toIgnore = canonical("").string().length() + 1;
	recursive_directory_iterator end = recursive_directory_iterator();
	while (it != end)
	{
		if ((*it).path().has_extension())
		{
			std::string found = (*it).path().string();
			myFoundFiles[(*it).path().extension().string()].push_back(found.substr(toIgnore));
		}
		++it;
	}
}
#endif // !_RETAIL
void GameState::RecieveMessage(const Message& aMessage)
{

	switch (aMessage.myMessageType)
	{
	case MessageType::FinnishGame:
	{
		myFinnishGameAfterFadeOut = true;

		/*Message msg;
		msg.myMessageType = MessageType::PopState;
		msg.aBool = true;
		PostMaster::GetInstance()->SendMessages(msg);*/
		break;
	}
	case MessageType::GoToSleep:
	{
		PostMaster::GetInstance()->SendMessages(MessageType::FadeOut);
	}
	break;
	case MessageType::FadeOutComplete:
	{
		PostMaster::GetInstance()->SendMessages(MessageType::NewDay);
		PostMaster::GetInstance()->SendMessages(MessageType::FadeIn);
	}
	break;

	case MessageType::PlayerDied:
	{
		Message mess;
		mess.myMessageType = MessageType::AnnounceDeathMarker;
		V3F pos = myGameWorld->GetPlayerPosition();
		mess.myData = &pos;
		PostMaster::GetInstance()->SendMessages(mess);
	}

	myGameWorld->RespawnPlayer(myLatestCheckpointPos);

	break;

	case MessageType::ChangeLevel:
		LoadLevel(aMessage.myIntValue);
		break;

	case MessageType::NextLevel:
		NextLevel();
		break;
	case MessageType::BulletHit:
	{
		BulletHitReport* report = (BulletHitReport*)aMessage.myData;
		if (!aMessage.myBool) // Did not hit entity
		{
			Decal* decal = myDecalFactory.LoadDecal("Data/Decals/BulletHole.json", myScene->GetMainCamera());
			decal->myCamera->SetPosition(report->position + report->normal * 50.f);
			decal->myCamera->LookAt(report->position);
			myScene->AddToScene(decal);
		}
	}
	break;
	case MessageType::LoadLevel:
		MergeLevel(aMessage.myText.data());
		break;
	case MessageType::UnloadLevel:
	{
		myCurrentLoadedLevels.erase(aMessage.myText.data());
	}
	break;
#if DONETWORK
	case MessageType::AnnounceDeathMarker:
	{
		DeathSpotMessage netmessage;
		netmessage.myCategory = DeathSpotMessage::Category::Announcement;
		netmessage.myPosition = *((V3F*)aMessage.myData);
		myClient.Send(netmessage);
	}
	break;
	case MessageType::ConnectedToServer:
	{
		DeathSpotMessage netmessage;
		netmessage.myCategory = DeathSpotMessage::Category::Request;
		myClient.Send(netmessage);
	}
	break;
#endif
	case MessageType::WinGameAndAlsoLife:
		WinGame();
		break;
	default:
		LOGWARNING("Unknown messagetype in GameState");
		break;
	}
}