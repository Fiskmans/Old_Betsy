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
#include <Random.h>
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
	PostMaster::GetInstance()->Subscribe(MessageType::ChangeLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::NextLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::BulletHit, this);
	PostMaster::GetInstance()->Subscribe(MessageType::LoadLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::UnloadLevel, this);
	PostMaster::GetInstance()->Subscribe(MessageType::WinGameAndAlsoLife, this);
	PostMaster::GetInstance()->Subscribe(MessageType::GoToSleep, this);
	myCurrentLevel = "";
	myIsMain = true;
	myCurrentLevelindex = 0;
	myGraphManager.Load();
	myNodePollingStation = nullptr;
	myFinnishGameAfterFadeOut = false;
	myHasRenderedAtleastOneFrame = false;
}

GameState::~GameState()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::FadeOutComplete, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::FinnishGame, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::PlayerDied, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::ChangeLevel, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::NextLevel, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::BulletHit, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::LoadLevel, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::UnloadLevel, this);
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

bool GameState::Init(WindowHandler* aWindowHandler, InputManager* aInputManager, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myIsGameWon = false;
	myIsMain = true;

	myContext = aFramework->GetContext();
	myFramework = aFramework;
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
	return true;
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
	if (myLoadingLevel.IsLoaded())
	{
		MergeQueuedLevelPartially(0.007f);
	}
}


void GameState::RunImGui(float aDeltatime)
{
#if  USEIMGUI
	PathFinder::GetInstance().Imgui();
	AssetManager::GetInstance().ImGui();

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
		if (!myLoadingLevel.IsValid())
		{
			myLoadingLevel = AssetManager::GetInstance().GetLevel(aFilePath);
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
	LevelParseResult& level = myLoadingLevel.GetAsLevel();
	while (level.myIsComplete)
	{
		if (level.myIsSuccessfull)
		{
			level.myIsComplete = false;
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
		switch (level.myStep)
		{
		case 0:
		{
			PERFORMANCETAG("Skybox");
			ModelInstance* skybox = AssetManager::GetInstance().GetSkybox(level.mySkyboxPath).InstansiateSkybox();
			if (skybox)
			{
				delete mySkybox;
				myScene->SetSkybox(skybox);
				mySkybox = skybox;
			}
			++level.myStep;
		}
		break;
		case 1:
		{
			PERFORMANCETAG("Decals");
			if (level.myCounter >= level.myDecals.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				auto& package = level.myDecals[level.myCounter];
				Decal* decal = myDecalFactory.LoadDecal(package.aFilePath, package.aRotation, package.aTranslation);
				myScene->AddToScene(decal);
				++level.myCounter;
			}
		}
		break;
		case 2:
		{
			{
				PERFORMANCETAG("Enemies");
				if (level.myCounter >= level.myEnemyObjects.size())
				{
					level.myCounter = 0;
					++level.myStep;
				}
				else
				{
					Message enemySpawnMessage;
					enemySpawnMessage.myMessageType = MessageType::SpawnEnemy;
					enemySpawnMessage.myData = &level.myEnemyObjects[level.myCounter];
					Publisher::SendMessages(enemySpawnMessage);
					++level.myCounter;
				}
			}
		}
		break;
		case 3:
		{
			PERFORMANCETAG("Triggers");
			if (level.myCounter >= level.myTriggerBuffer.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				Message triggerSpawnMessage;
				triggerSpawnMessage.myMessageType = MessageType::SpawnTriggerBox;
				triggerSpawnMessage.myData = &level.myTriggerBuffer[level.myCounter];
				Publisher::SendMessages(triggerSpawnMessage);
				++level.myCounter;
			}
		}
		break;
		case 4:
		{
			PERFORMANCETAG("Waypoints");
			if (level.myCounter >= level.myMayaPositions.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				myNodePollingStation->AddMayaPos(level.myMayaPositions[level.myCounter].first, level.myMayaPositions[level.myCounter].second);
				++level.myCounter;
			}
		}
		break;
		case 5:
		{
			PERFORMANCETAG("Environmentlight");
			EnvironmentLight* envoLight = myLightLoader->LoadLight(level.mySkyboxPath.substr(0, level.mySkyboxPath.length() - 4) + "_light.dds");

			if (envoLight)
			{
				envoLight->myDirection = V3F(-level.myEnvironmentlight.direction[0], -level.myEnvironmentlight.direction[1], -level.myEnvironmentlight.direction[2]);
				envoLight->myColor = V3F(level.myEnvironmentlight.color[0], level.myEnvironmentlight.color[1], level.myEnvironmentlight.color[2]);
				envoLight->myIntensity = level.myEnvironmentlight.intensity;
				myScene->SetEnvironmentLight(envoLight);
			}
			++level.myStep;
		}
		break;
		case 6:
		{
			PERFORMANCETAG("Pointlights");
			if (level.myCounter >= level.myPointLights.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				Message triggerLightSpawnMessage;
				triggerLightSpawnMessage.myMessageType = MessageType::SpawnPointLight;
				triggerLightSpawnMessage.myData = &level.myPointLights[level.myCounter];
				Publisher::SendMessages(triggerLightSpawnMessage);
				++level.myCounter;
			}
		}
		break;
		case 7:
		{
			PERFORMANCETAG("NavMesh");
			PathFinder::GetInstance().SetNavMesh(AssetManager::GetInstance().GetNavMesh(level.myNavMeshPath));
#if 0
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
			}
#endif
			++level.myStep;
		}
		break;
		case 8:
		{
			PERFORMANCETAG("CollisionBoxes");
			if (level.myCounter >= level.myCollsionBoxes.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				Message spawnGBPhysXStaticMessage;
				spawnGBPhysXStaticMessage.myMessageType = MessageType::SpawnGBPhysXBox;
				spawnGBPhysXStaticMessage.myData = &level.myCollsionBoxes[level.myCounter];
				Publisher::SendMessages(spawnGBPhysXStaticMessage);
				++level.myCounter;
			}
		}
		break;
		case 9:
		{
			PERFORMANCETAG("Destructables");
			if (level.myCounter >= level.myDestrucables.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				Message destructibleSpawnMessage;
				destructibleSpawnMessage.myMessageType = MessageType::SpawnDestructibleObject;
				destructibleSpawnMessage.myData = &level.myDestrucables[level.myCounter];
				destructibleSpawnMessage.myBool = false;
				Publisher::SendMessages(destructibleSpawnMessage);
				++level.myCounter;
			}
		}
		break;
		case 10:
		{
			PERFORMANCETAG("Spotlights");
			if (level.myCounter >= level.mySpotlights.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				auto& objBuffer = level.mySpotlights[level.myCounter];
				SpotLight* l = mySpotlightFactory.LoadSpotlight(objBuffer.aFilePath, objBuffer.aFov, objBuffer.aRange, objBuffer.aIntensity, objBuffer.aRotation, objBuffer.aTranslation);
				myScene->AddToScene(l);
				++level.myCounter;
			}
		}
		break;
		case 11:
		{
			PERFORMANCETAG("Interactable objects");
			if (level.myCounter >= level.myInteractables.size())
			{
				level.myCounter = 0;
				++level.myStep;
			}
			else
			{
				Message spawnInteractableMessage;
				spawnInteractableMessage.myMessageType = MessageType::SpawnInteractable;
				spawnInteractableMessage.myData = &level.myInteractables[level.myCounter];
				Publisher::SendMessages(spawnInteractableMessage);
				++level.myCounter;
			}
		}
		break;
		case 12:
		{
			{
				PERFORMANCETAG("Static objects");
				if (level.myCounter >= level.myStaticObjects.size())
				{
					level.myCounter = 0;
					++level.myStep;
				}
				else
				{
					Message spawnStaticMessage;
					spawnStaticMessage.myMessageType = MessageType::SpawnStaticObject;
					spawnStaticMessage.myData = &level.myStaticObjects[level.myCounter];
					Publisher::SendMessages(spawnStaticMessage);
					++level.myCounter;
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
			myCurrentLevel = level.myLevelFile;

			Message newLevelLoadedMessage;
			newLevelLoadedMessage.myMessageType = MessageType::NewLevelLoaded;
			newLevelLoadedMessage.myIntValue = level.myWorldAxisSize;
			newLevelLoadedMessage.myIntValue2 = myCurrentLevelindex;
			newLevelLoadedMessage.myData = &level.myPlayerStart;
			Publisher::SendMessages(newLevelLoadedMessage);
			myGraphManager.ReTriggerTree();

			myLatestCheckpointPos = level.myPlayerStart;

			level = LevelParseResult();
			level.myIsComplete = false;
		}
		break;
		}
	}

	return true;
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
	myGameWorld->SystemLoad( mySpriteFactory, myScene, aFramework, aAudioManager, myGBPhysX, aSpriteRenderer, myLightLoader);

	myGameWorld->Init(mySpriteFactory, myScene, aFramework, camera, myNodePollingStation, &mySpotlightFactory);	//LoadDefaultLevel();
	myNodePollingStation->SetTimerController(&myTimerController);
	myNodePollingStation->SetParticleFactory(myGameWorld->GetParticleFactory());
	myNodePollingStation->SetScene(myScene);
	myNodePollingStation->SetSpriteFactory(mySpriteFactory);

#if USEIMGUI
	std::thread thread(std::bind(&GameState::SearchForFiles, this));
	thread.detach();
#endif

	FiskJSON::Object& levelSequence = AssetManager::GetInstance().GetJSON("data/levelsequence.json").GetAsJSON();

	for (auto& i : levelSequence["levels"].Get<FiskJSON::Array>())
	{
		myLevelSequence.push_back(i->Get<std::string>());
	}
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

	if (myLoadingLevel.IsValid() && myLoadingLevel.IsLoaded())
	{
		if (myHasRenderedAtleastOneFrame)
		{
			MergeQueuedLevelPartially(INFINITY);
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

	myTimerController.CheckTimers();


#if USEFILEWATHCER
	{
		PERFORMANCETAG("FileWatchers");
		myWatcher.FlushChanges();
		AssetManager::GetInstance().FlushChanges();
	}
#endif // USEFILEWATCHER


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