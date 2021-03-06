#pragma once
#include "pch.h"
#include "GameState.h"
#include <GraphicEngine.h>
#include "RenderScene.h"
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

#include "GamlaBettan\EntityManager.h"
#include "GamlaBettan\Terrain.h"

GameState::GameState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop),
	mySpriteFactory(nullptr),
	mySkybox(nullptr),
	myLightLoader(nullptr),
	Observer(
		{
			MessageType::ChangeLevel,
			MessageType::NextLevel,
			MessageType::LoadLevel,
			MessageType::UnloadLevel
		})
{ 
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
	myInputManager = nullptr;
	myCurrentLevel = "";
	myIsMain = true;
	myCurrentLevelindex = 0;
	myNodePollingStation = nullptr;
	myHasRenderedAtleastOneFrame = false;
	myLastFrame = Tools::GetTotalTime();
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

bool GameState::Init(InputManager* aInputManager, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager)
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

	//Terrain::GetInstance().Init(aFramework, myGBPhysX);
	CreateWorld(aFramework);



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

			EnvironmentLight* oldLight = RenderScene::GetInstance().GetEnvironmentLight();
			RenderScene::GetInstance().SetEnvironmentLight(light);
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

void GameState::PreSetup()
{
	if (myLoadingLevel.IsLoaded())
	{
		MergeQueuedLevelPartially(0.007f);
	}
}


void GameState::RunImGui()
{
#if  USEIMGUI
	PathFinder::GetInstance().Imgui();
	AssetManager::GetInstance().ImGui();
	EntityManager::GetInstance().ImGui();

	// ImGui
	ModelInstance* toRemove = nullptr;


	static bool editGraphs = false;
	static bool drawFrustums = false;
	if (drawFrustums)
	{
		for (auto i : RenderScene::GetInstance().GetDecals())
		{
			DebugDrawer::GetInstance().DrawFrustum(i->myCamera->GenerateFrustum());
		}
	}

	{
		PERFORMANCETAG("Gamestate imgui");
		WindowControl::Window("Game", [&]()
			{
				ImguiContent(drawFrustums);
			});
		Logger::RapportWindow();
	}

	//GAMEWORLD UPDATE --------------------------------------

	{
		PERFORMANCETAG("Gameworld update")
		myGameWorld->Update();
	}

	// Model Remove -----------------------------------------

	if (toRemove)
	{
		RenderScene::GetInstance().RemoveFromScene(toRemove);
	}

#endif // !_RETAIL
}

void GameState::ImguiContent(bool& drawFrustums)
{
	ImGui::Checkbox("Visualize decals", &drawFrustums);

	ImGui::Separator();

	EnvironmentLight* light = RenderScene::GetInstance().GetEnvironmentLight();
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
		RenderScene::GetInstance().GetMainCamera()->SetFov(fov);
		lastFov = fov;
	}

	if (ImGui::CollapsingHeader("Scene"))
	{
		if (ImGui::Button("Remove all"))
		{
			RenderScene::GetInstance().RemoveAll();
		}
		if (ImGui::TreeNode("Lights"))
		{
			int counter = 0;
			for (PointLight* i : RenderScene::GetInstance().GetPointLights())
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
			int counter = 0;
			for (auto& SpriteInstance : RenderScene::GetInstance().GetSprites())
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
		CommonUtilities::Vector3<float> pos = RenderScene::GetInstance().GetMainCamera()->GetPosition();
		ImGui::BulletText("Camera position X:%.2f Y:%.2f Z:%.2f", pos.x, pos.y, pos.z);
		ImGui::BulletText("Mouse screen position X:%.2f Y:%.2f", myInputManager->GetMouseNomalizedX(), myInputManager->GetMouseNomalizedY());
	}
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
				RenderScene::GetInstance().SetSkybox(skybox);
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
				RenderScene::GetInstance().AddToScene(decal);
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
				RenderScene::GetInstance().SetEnvironmentLight(envoLight);
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
				RenderScene::GetInstance().AddToScene(l);
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
			for (auto& i : RenderScene::GetInstance())
			{
				i->SetIsHighlighted(false);
			}
			myCurrentLevel = level.myLevelFile;

			MessageStructs::LevelLoaded data
			{
				level.myWorldAxisSize,
				static_cast<int>(myCurrentLevelindex),
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
	RenderScene::GetInstance().RemoveAll();
	//TEMPORARY PHYSX WORLD CLEAN probably to be exchanged for entities cleaning their own physx actor
	myGBPhysX->GBResetScene();
}

void GameState::UnloadLevel(std::string aFilepath)
{
	PostMaster::GetInstance().SendMessages(MessageType::UnloadLevel, aFilepath.c_str());
}

void GameState::CreateWorld(DirectX11Framework* aFramework)
{
	myGameWorld = new GameWorld(mySpriteFactory, myFramework, myAudioManager, myGBPhysX);
	myDevice = aFramework->GetDevice();

	Camera* camera = CCameraFactory::CreateCamera(90.f, true);
	camera->SetPosition(V3F(0, 0, -1000.f));
	camera->SetFov(81.f);

	RenderScene::GetInstance().SetMainCamera(camera);
#ifdef _DEBUG
	DebugTools::myCamera = camera;
	DebugTools::Setup();
#endif // _DEBUG

	FiskJSON::Object& levelSequence = AssetManager::GetInstance().GetJSON("data/levelsequence.json").GetAsJSON();

	for (auto& i : levelSequence["levels"].Get<FiskJSON::ArrayWrapper>())
	{
		myLevelSequence.push_back(i.Get<std::string>());
	}
}

void GameState::Activate()
{
}

void GameState::Deactivate()
{
}

void GameState::Unload()
{
	RenderScene::GetInstance().RemoveAll();
}

void GameState::Update()
{
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
	float now = Tools::GetTotalTime();
	float dt = now - myLastFrame;
	myLastFrame = now;

	WindowControl::Window("Fourier", [this, now]()
		{
			ImguiContent(now);
		});

	myTimerController.CheckTimers();


#if USEFILEWATHCER
	{
		PERFORMANCETAG("FileWatchers");
		myWatcher.FlushChanges();
		AssetManager::GetInstance().FlushChanges();
	}
#endif // USEFILEWATCHER


#if USEIMGUI
	RunImGui(); // calls mygameworld.Update inside so gameworld gets a tab inside the window;
#if !BAKEVALUES
	{
		PERFORMANCETAG("Metric editor")
		MetricHandler::ImGuiValueEditor();
	}
#endif
#else
	myGameWorld->Update(*myInputManager->GetInputHandler(), aDeltaTime);
#endif

	{
		PERFORMANCETAG("TERRAIN");
		//Terrain::GetInstance().Update();
#if USEIMGUI
		//Terrain::GetInstance().Imgui();
#endif
	}

	{
		PERFORMANCETAG("Components");
		ComponentBase::FrameData data
		{
			*myInputManager->GetInputHandler(),
			dt,
			Tools::GetTotalTime()
		};

		ComponentManager::GetInstance().Update(data);
	}

	{
		PERFORMANCETAG("PhysX");
		if (myGBPhysX->GetGBPhysXActive())
		{
			myGBPhysX->GBStepPhysics(dt);
		}
	}

	{
		PERFORMANCETAG("Graphical updates");
		RenderScene::GetInstance().Update(dt);
	}
}

void GameState::ImguiContent(float now)
{
	static std::vector<CompF> original;

	static std::vector<CompF> dataPoints;
	static int precision = 4;
	static float speedScale = 1.f;
	static float ZoomScale = 10.f;
	float time = fmodf(now / 5.f, 1.f);

	static bool IsDrawing = false;

	if (original.empty() && !IsDrawing)
	{
		CompF top = CompF(0.f, ImagF(0.f));
		CompF left = CompF(-1.f, ImagF(-2.f));
		CompF right = CompF(1.f, ImagF(-2.f));

		for (float i = 0.1f; i < 1.f; i += 0.01f)
		{
			original.push_back(LERP(top, left, i));
		}
		for (float i = 0.f; i < 1.f; i += 0.01f)
		{
			original.push_back(LERP(left, right, i));
		}
		for (float i = 0.f; i < 0.9f; i += 0.01f)
		{
			original.push_back(LERP(right, top, i));
		}

		CompF center = CompF(0.f, ImagF(2.f));
		float radius = 1.6f;

		CompF cirbeg = CompF::EPow(TAU_F * (0.1f) - PI_F / 2.f) * radius + center;
		CompF tribeg = LERP(right, top, 0.9f);

		for (float i = 0; i < 1.0f; i += 0.01f)
		{
			original.push_back(LERP(tribeg, cirbeg, i));
		}

		for (float i = 0; i < 1.f; i += 0.001f)
		{
			original.push_back(CompF::EPow(TAU_F * (i * 0.8f + 0.1f) - PI_F / 2.f) * radius + center);
		}

		CompF cirend = CompF::EPow(TAU_F * (0.9f) - PI_F / 2.f) * radius + center;
		CompF triend = LERP(top, left, 0.1f);

		for (float i = 0; i < 1.0f; i += 0.01f)
		{
			original.push_back(LERP(cirend, triend, i));
		}
	}





	static std::vector<Math::FourierTransformValue> series = Math::DiscreteFourierTransform(original, precision);

	dataPoints.push_back(Math::DescreteFourierInterpolate(series, time));

	std::vector<CompF> steps = Math::DescreteFourierInterpolateSteps(series, time);

	V3F offset = V3F(0, 300, 0);
	V3F xaxis = V3F(200, 0, 0);
	V3F yaxis = V3F(0, 0, 200);

#ifdef _DEBUG
	if (IsDrawing)
	{
		if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && DebugTools::LastKnownMouseRay)
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

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && DebugTools::LastKnownMouseRay)
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
		V3F newOffset = V3F(0.f, 50.f, 0.f) + offset - x * newXaxis - y * newYaxis;

		DebugDrawer::GetInstance().SetColor(V4F(0.f, 1.f, 0.f, 1.f));
		DebugDrawer::GetInstance().DrawAnyLines(newOffset, { newXaxis, newYaxis }, reinterpret_cast<float*>(original.data()), original.size());
		DebugDrawer::GetInstance().SetColor(V4F(0.7f, 0.7f, 1.f, 1.f));
		DebugDrawer::GetInstance().DrawAnyLines(newOffset, { newXaxis, newYaxis }, reinterpret_cast<float*>(steps.data()), steps.size());
		DebugDrawer::GetInstance().SetColor(V4F(1.f, 0.f, 0.f, 1.f));
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

		DebugDrawer::GetInstance().SetColor(V4F(0.7f, 1.f, 0.4f, 1.f));
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


	DebugDrawer::GetInstance().SetColor(V4F(0.f, 1.f, 0.f, 1.f));
	DebugDrawer::GetInstance().DrawAnyLines(offset, { xaxis, yaxis }, reinterpret_cast<float*>(original.data()), original.size());
	DebugDrawer::GetInstance().SetColor(V4F(0.7f, 0.7f, 1.f, 1.f));
	DebugDrawer::GetInstance().DrawAnyLines(offset, { xaxis, yaxis }, reinterpret_cast<float*>(steps.data()), steps.size());
	DebugDrawer::GetInstance().SetColor(V4F(1.f, 0.f, 0.f, 1.f));
	DebugDrawer::GetInstance().DrawAnyLines(offset, { xaxis, yaxis }, reinterpret_cast<float*>(dataPoints.data()), dataPoints.size());
#endif

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
	}
}

void GameState::RecieveMessage(const Message& aMessage)
{

	switch (aMessage.myMessageType)
	{
	case MessageType::ChangeLevel:
		LoadLevel(*reinterpret_cast<const int*>(aMessage.myData));
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