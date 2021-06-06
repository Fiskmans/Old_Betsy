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

#include "AudioManager.h"

#include "ModelInstance.h"
#include "Model.h"
#include "CameraFactory.h"
#include "Camera.h"
#include "LightLoader.h"
#include "Environmentlight.h"
#include "PointLight.h"
#include <DirectX11Framework.h>
#include "SpriteInstance.h"
#include <DirectXTK\Inc\ScreenGrab.h>
#include <FullscreenRenderer.h>

#include <PathFinder.h>

#include <GBPhysX.h>

#include "AssetManager.h"

#include "Complex.h"
#include "Fourier.h"

GameState::GameState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop),
	mySpriteFactory(nullptr),
	mySkybox(nullptr),
	myLightLoader(nullptr),
	Observer(
		{
			MessageType::FadeOutComplete,
			MessageType::FinnishGame,
			MessageType::ChangeLevel,
			MessageType::NextLevel,
			MessageType::BulletHit,
			MessageType::LoadLevel,
			MessageType::UnloadLevel,
			MessageType::WinGameAndAlsoLife,
			MessageType::GoToSleep
		})
{
	myGameWorld = new GameWorld();
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
	myInputManager = nullptr;
	myCurrentLevel = "";
	myIsMain = true;
	myCurrentLevelindex = 0;
	myNodePollingStation = nullptr;
	myHasRenderedAtleastOneFrame = false;
}

GameState::~GameState()
{

#if USEFILEWATHCER
	myWatcher.UnRegister(myMetricHandle);
#endif // !_RETAIL
	SAFE_DELETE(myGameWorld);
	if (myGBPhysX)
	{
		myGBPhysX->GBCleanUpPhysics();
	}
	SAFE_DELETE(myGBPhysX);
}

bool GameState::Init(InputManager* aInputManager, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myIsMain = true;

	myContext = aFramework->GetContext();
	myFramework = aFramework;
	mySpriteFactory = aSpritefactory;
	myLightLoader = aLightLoader;
	myInputManager = aInputManager;
	myAudioManager = aAudioManager;
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

	CreateWorld(aFramework, aAudioManager, aSpriteRenderer);



#if !BAKEVALUES
	MetricHandler::Load(METRICFILE);
#if USEFILEWATHCER
	myMetricHandle = myWatcher.RegisterCallback(METRICFILE, MetricHandler::Load);
#endif // !_RETAIL
#endif

	return true;
}

void GameState::Render(CGraphicsEngine* aGraphicsEngine)
{
	PathFinder::GetInstance().DrawDebug();
	aGraphicsEngine->RenderFrame();

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

			EnvironmentLight* oldLight = Scene::GetInstance().GetEnvironmentLight();
			Scene::GetInstance().SetEnvironmentLight(light);
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

	int level = myCurrentLevelindex;
	PostMaster::GetInstance().SendMessages(MessageType::CurrentLevel, &level);
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


	static bool editGraphs = false;
	static bool drawFrustums = false;
	if (drawFrustums)
	{
		for (auto i : Scene::GetInstance().GetDecals())
		{
			DebugDrawer::GetInstance().DrawFrustum(i->myCamera->GenerateFrustum());
		}
	}

	{
		PERFORMANCETAG("Gamestate imgui");
		WindowControl::Window("Game", [&]()
			{

				ImGui::Checkbox("Visualize decals", &drawFrustums);

				ImGui::Separator();

				EnvironmentLight* light = Scene::GetInstance().GetEnvironmentLight();
				if (light)
				{
					ImGui::ColorEdit3("Light Color", &light->myColor.x);
					ImGui::DragFloat3("Light Direction", &light->myDirection.x, 0.01f);
					ImGui::DragFloat("Light Intensity", &light->myIntensity, 0.01f);
				}
				static float fov = 81;
				static float lastFov;
				ImGui::DragFloat("Camera FOV", &fov, 0.1f, 0.f, 170.f);
				if (fov != lastFov)
				{
					Scene::GetInstance().GetMainCamera()->SetFov(fov);
					lastFov = fov;
				}

				if (ImGui::CollapsingHeader("Scene"))
				{
					if (ImGui::Button("Remove all"))
					{
						Scene::GetInstance().RemoveAll();
					}
					if (ImGui::TreeNode("Meshes"))
					{
						for (ModelInstance* i : Scene::GetInstance())
						{
							if (ImGui::TreeNode(i->GetFriendlyName().c_str()))
							{
								if (i->ImGuiNode(myFoundFiles, Scene::GetInstance().GetMainCamera()))
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
						for (PointLight* i : Scene::GetInstance().GetPointLights())
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
						for (auto& SpriteInstance : Scene::GetInstance().GetSprites())
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

				myGameWorld->ImGuiNode();
				if (ImGui::CollapsingHeader("Tools"))
				{
					if (ImGui::Button("Clear log"))
					{
						system("cls");
					}
				}
				if (ImGui::CollapsingHeader("Info"))
				{
					CommonUtilities::Vector3<float> pos = Scene::GetInstance().GetMainCamera()->GetPosition();
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

	// Model Remove -----------------------------------------

	if (toRemove)
	{
		Scene::GetInstance().RemoveFromScene(toRemove);
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
			PostMaster::GetInstance().SendMessages(MessageType::StartLoading, aFilePath.c_str());
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
				Scene::GetInstance().SetSkybox(skybox);
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
				Scene::GetInstance().AddToScene(decal);
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
					SYSERROR("Level contains enemies, no longer supported");
					level.myCounter = 0;
					++level.myStep;
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
				SYSERROR("Level contains triggers, no longer supported");
				level.myCounter = 0;
				++level.myStep;
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
				SYSWARNING("Level contains waypoints, system discontinued");
				level.myCounter = 0;
				++level.myStep;
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
				Scene::GetInstance().SetEnvironmentLight(envoLight);
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
				SYSERROR("Level contains pointlights, no longer supported");
				level.myCounter = 0;
				++level.myStep;
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
				SYSERROR("Level contains collision boxes, no longer supported");
				level.myCounter = 0;
				++level.myStep;
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
				SYSERROR("Level contains desttructables, no longer supported");
				level.myCounter = 0;
				++level.myStep;
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
				Scene::GetInstance().AddToScene(l);
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
				SYSERROR("Level contains interactables, no longer supported");
				level.myCounter = 0;
				++level.myStep;
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
					SYSERROR("Level contains static objects, no longer supported");
					level.myCounter = 0;
					++level.myStep;
				}
			}
		}
		break;
		default:
		{
			PERFORMANCETAG("Finishing up");
			for (auto& i : Scene::GetInstance())
			{
				i->SetIsHighlighted(false);
			}
			myCurrentLevel = level.myLevelFile;

			MessageStructs::LevelLoaded data
			{
				level.myWorldAxisSize,
				myCurrentLevelindex,
				level.myPlayerStart
			};
			PostMaster::GetInstance().SendMessages(MessageType::NewLevelLoaded,&data);

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
	Scene::GetInstance().RemoveAll();
	//TEMPORARY PHYSX WORLD CLEAN probably to be exchanged for entities cleaning their own physx actor
	myGBPhysX->GBResetScene();
}

void GameState::UnloadLevel(std::string aFilepath)
{
	PostMaster::GetInstance().SendMessages(MessageType::UnloadLevel, aFilepath.c_str());
}

void GameState::CreateWorld(DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myDevice = aFramework->GetDevice();

	Camera* camera = CCameraFactory::CreateCamera(90.f, true);
	camera->SetPosition(V3F(0, 0, -1000.f));
	camera->SetFov(81.f);

	Scene::GetInstance().SetMainCamera(camera);
#ifdef _DEBUG
	DebugTools::myCamera = camera;
	DebugTools::Setup();
#endif // _DEBUG
	myGameWorld->SystemLoad(mySpriteFactory, aFramework, aAudioManager, myGBPhysX, myLightLoader);
	myGameWorld->Init(mySpriteFactory, aFramework);

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

void GameState::Activate()
{
	PostMaster::GetInstance().SendMessages(MessageType::GameActive);
}

void GameState::Deactivate()
{
	PostMaster::GetInstance().SendMessages(MessageType::GameNotActive);
}

void GameState::Unload()
{
	Scene::GetInstance().RemoveAll();
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
					V3F intersect;
					DebugTools::LastKnownMouseRay->FindIntersection(p, intersect);
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
				V3F intersect;
				DebugTools::LastKnownMouseRay->FindIntersection(p, intersect);
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
			ImGui::InputFloat("Speed", &speedScale, 0.01f, 0.03f);
			ImGui::InputFloat("ZoomScale", &ZoomScale, 1.f, 2.0f);

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
			myGBPhysX->GBStepPhysics(aDeltaTime);
		}
	}

	{
		PERFORMANCETAG("Graphical updates");
		Scene::GetInstance().Update(aDeltaTime);
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
	case MessageType::GoToSleep:
	{
		PostMaster::GetInstance().SendMessages(MessageType::FadeOut);
	}
	break;
	case MessageType::FadeOutComplete:
	{
		PostMaster::GetInstance().SendMessages(MessageType::NewDay);
		PostMaster::GetInstance().SendMessages(MessageType::FadeIn);
	}
	break;


	case MessageType::ChangeLevel:
		LoadLevel(*reinterpret_cast<const int*>(aMessage.myData));
		break;
	case MessageType::BulletHit:
	{
		BulletHitReport* report = (BulletHitReport*)aMessage.myData;
		if (!*reinterpret_cast<const bool*>(aMessage.myData)) // Did not hit entity
		{
			Decal* decal = myDecalFactory.LoadDecal("Data/Decals/BulletHole.json", Scene::GetInstance().GetMainCamera());
			decal->myCamera->SetPosition(report->position + report->normal * 50.f);
			decal->myCamera->LookAt(report->position);
			Scene::GetInstance().AddToScene(decal);
		}
	}
	break;
	case MessageType::LoadLevel:
		MergeLevel(reinterpret_cast<const char*>(aMessage.myData));
		break;
	case MessageType::UnloadLevel:
	{
		myCurrentLoadedLevels.erase(reinterpret_cast<const char*>(aMessage.myData));
	}
	break;
	default:
		LOGWARNING("Unknown messagetype in GameState");
		break;
	}
}