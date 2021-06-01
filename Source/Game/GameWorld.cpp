#include <pch.h>
#include "GameWorld.h"
#include "Camera.h"
#include "InputHandler.h"
#include "Mesh.h"
#include "Movement3D.h"
#include "AnimationComponent.h"
#include "GBPhysXComponent.h"
#include "GBPhysXKinematicComponent.h"
#include "FPSCamera.h"
#include "FireWeapon.h"

#include "PlayerController.h"
#include "light.h"
#include "Life.h"
#include "FollowCamera.h"

#include "..//CommonUtilities/Vector3.hpp"
#include <ModelLoader.h>
#include <Scene.h>
#include "Octree.h"
#include "IntersectionRecord.h"

#include "ComponentLake.h"
#include "Sphere.hpp"
#include "DebugDrawer.h"
#include "Random.h"
#include <DebugTools.h>
#include <experimental/filesystem>
#include "Model.h"
#include "Audio.h"
#include "ShaderFlags.h"
#include "Animator.h"
#include "AIController.h"
#include "CharacterInstance.h"

#include "ParticleActivatable.h"

#include "StaticObjectFactory.h"
#include "DynamicObjectFactory.h"
#include "DestructibleFactory.h"
#include "InteractableFactory.h"
#include "TriggerBoxFactory.h"
#include "PointLightFactory.h"
#include "ParticleFactory.h"
#include <SpriteFactory.h>
#include <SpriteInstance.h>

#include "AudioManager.h"

#include "TextFactory.h"

#include "GBPhysX.h"
#include "GBPhysXColliderFactory.h"

#include "TextInstance.h"

#include "UIManager.h"
#include "CharacterData.h"
#include <WindSystem.h>
#include "perlin_noise.h"

#if USEIMGUI
#include <imgui.h>
#include <WindowControl.h>
#include <ModelLoader.h>
#include <LightLoader.h>
#endif 

#include <Environmentlight.h>
#include <Intersection.hpp>
#include "ParticleInstance.h"
#include <SpotLight.h>
#include <SpotLightFactory.h>
#include "LifeTime.h"
#include "GBPhysXStaticComponent.h"
#include "PointLight.h"
#include "Growable.h"
#include "GrowthSpot.h"
#include "House.h"
#include "Inventory.h"
#include "FoodCellar.h"
#include "DialogFactory.h"
#include "Trader.h"
#include "TraderAI.h"
#include "WildPlant.h"

#include "DirectXTK\Inc\SpriteBatch.h"
#include "DirectX11Framework.h"

#include "AssetManager.h"

GameWorld::GameWorld() :
	myEntityID(0),
	myUsingFreeCamera(true),
	myPlayer(nullptr),
	myCamera(nullptr),
	myObjectTree(nullptr),
	myScenePtr(nullptr),
	myParticleFactory(nullptr),
	myStaticObjectFactory(nullptr),
	myDynamicObjectFactory(nullptr),
	myDestructibleFactory(nullptr),
	myTriggerBoxFactory(nullptr),
	myPointLightFactory(nullptr),
	myGBPhysXPtr(nullptr),
	myUIManager(nullptr),
	myTextFactory(nullptr),
	myCharacterData(nullptr),
	myGBPhysXColliderFactory(nullptr),
	myDialogFactory(nullptr),
#if USEIMGUI
	myLastPackage(nullptr),
#endif 
	myMainCameraPtr(nullptr),
	myAIPollingStation(nullptr),
	myModelLoader(nullptr),
	myTrader(nullptr),
	mySpriteFactory(nullptr),
	myPlayerSpotlight(nullptr),
	myRiverAudioEntity(nullptr),
	mySpotlightFactory(nullptr)
{
	myEntityPool.Init(ENTITY_POOL_SIZE);
}

GameWorld::~GameWorld()
{
	ClearWorld(true);

	UnSubscribeToMessage(MessageType::NewLevelLoaded);
	UnSubscribeToMessage(MessageType::WindowResize);
	UnSubscribeToMessage(MessageType::EnemyDied);
	UnSubscribeToMessage(MessageType::TriggerEvent);
	UnSubscribeToMessage(MessageType::FadeInComplete);
	UnSubscribeToMessage(MessageType::StartDialogue);
	UnSubscribeToMessage(MessageType::DialogueOver);
	UnSubscribeToMessage(MessageType::RespawnTrader);
	UnSubscribeToMessage(MessageType::DespawnTrader);
	UnSubscribeToMessage(MessageType::RiverCreated);
	UnSubscribeToMessage(MessageType::FadeOut);
	UnSubscribeToMessage(MessageType::FadeOutComplete);
	UnSubscribeToMessage(MessageType::CreateSeed);
	UnSubscribeToMessage(MessageType::RequestUISetupPtrs);
	UnSubscribeToMessage(MessageType::StartInGameAudio);
	UnSubscribeToMessage(MessageType::DialogueOver);

	myEntityPool.Release();

	myScenePtr = nullptr;
	myMainCameraPtr = nullptr;
	myGBPhysXPtr = nullptr;

	SAFE_DELETE(myObjectTree);
	SAFE_DELETE(myStaticObjectFactory);
	SAFE_DELETE(myDynamicObjectFactory);
	SAFE_DELETE(myDestructibleFactory);
	SAFE_DELETE(myTriggerBoxFactory);
	SAFE_DELETE(myInteractableFactory);
	SAFE_DELETE(myPointLightFactory);
	SAFE_DELETE(myParticleFactory);
	SAFE_DELETE(myUIManager);
	SAFE_DELETE(myCharacterData);
	SAFE_DELETE(myTextFactory);
	SAFE_DELETE(myGBPhysXColliderFactory);
	SAFE_DELETE(myAIPollingStation);
	SAFE_DELETE(myTextFactory);
	SAFE_DELETE(myDialogFactory);
}

void GameWorld::SystemLoad(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, SpriteRenderer* aSpriteRenderer, LightLoader* aLightLoader)
{
	myWindowSize.x = 1920;
	myWindowSize.y = 1080;
	mySpriteFactory = aSpriteFactory;

	aAudioManager->SetMasterVolume(40.0f);
	aAudioManager->Register2DMasterObject(myEntityID++);

	myEntitys.reserve(ENTITY_POOL_SIZE);
	myLights.reserve(ENTITY_POOL_SIZE);
	myTriggers.reserve(ENTITY_POOL_SIZE);
	myEnemies.reserve(ENTITY_POOL_SIZE);
	myAnimations.reserve(ENTITY_POOL_SIZE);

	myParticleFactory = new ParticleFactory;
	myStaticObjectFactory = new StaticObjectFactory;
	myDynamicObjectFactory = new DynamicObjectFactory;
	myDestructibleFactory = new DestructibleFactory;
	myInteractableFactory = new InteractableFactory();
	myTriggerBoxFactory = new TriggerBoxFactory;
	myPointLightFactory = new PointLightFactory;
	myTextFactory = new TextFactory;
	myDialogFactory = new DialogFactory;

	myGBPhysXPtr = aGBPhysX;
	myGBPhysXColliderFactory = new GBPhysXColliderFactory;

	myUIManager = new UIManager;

	myCharacterData = new CharacterData();
	myCharacterData->Load();

	myAIPollingStation = new AIPollingStation;

	myParticleFactory->Init(aFramework);
	myTextFactory->Init(aSpriteRenderer, aSpriteFactory);
	myDialogFactory->Init(aSpriteRenderer, aSpriteFactory);

	ComponentLake::GetInstance().RegisterComponents();
	ComponentLake::GetInstance().PrepareObjectsInPools(aScene, myParticleFactory, aAudioManager, aLightLoader, &myEntitys, &myEntityPool, aSpriteFactory);

	myStaticObjectFactory->Init(myGBPhysXPtr, myObjectTree, &myEntitys, &myEntityPool, &ComponentLake::GetInstance(), &myEntityID, myUIManager);
	myDynamicObjectFactory->Init(myGBPhysXPtr, myObjectTree, &myEntitys, &myEntityPool, &ComponentLake::GetInstance(), &myEntityID);
	myDestructibleFactory->Init(myObjectTree, &myEntitys, &myEntityPool, &ComponentLake::GetInstance(), &myEntityID, myCharacterData);
	myInteractableFactory->Init(myGBPhysXPtr, myObjectTree, &myEnemies, &myEntityPool, &ComponentLake::GetInstance(), &myEntityID);
	myTriggerBoxFactory->Init(myObjectTree, &myTriggers, &myEntityPool, &ComponentLake::GetInstance(), &myEntityID);
	myPointLightFactory->Init(&myLights, &myEntityPool, &ComponentLake::GetInstance(), &myEntityID);
	myGBPhysXColliderFactory->Init(myGBPhysXPtr);

	FoodCellar::PopulateCalorieLookup();
	GrowthSpot::PopulateSeeds();

	TimeHandler::GetInstance().SetEntityVectors(&myAnimations, &myEnemies, &myEntityPool);
	TimeHandler::GetInstance().Init();
}

void GameWorld::Init(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework, Camera* aCamera, SpotLightFactory* aSpotlightFactory)
{
	myMainCameraPtr = aCamera;
	myScenePtr = aScene;
	mySpotlightFactory = aSpotlightFactory;
	mySpriteFactory = aSpriteFactory;

	SubscribeToMessage(MessageType::NewLevelLoaded);
	SubscribeToMessage(MessageType::WindowResize);
	SubscribeToMessage(MessageType::EnemyDied);
	SubscribeToMessage(MessageType::TriggerEvent);
	SubscribeToMessage(MessageType::FadeInComplete);
	SubscribeToMessage(MessageType::StartDialogue);
	SubscribeToMessage(MessageType::DialogueOver);
	SubscribeToMessage(MessageType::RespawnTrader);
	SubscribeToMessage(MessageType::DespawnTrader);
	SubscribeToMessage(MessageType::RiverCreated);
	SubscribeToMessage(MessageType::FadeOut);
	SubscribeToMessage(MessageType::FadeOutComplete);
	SubscribeToMessage(MessageType::CreateSeed);
	SubscribeToMessage(MessageType::RequestUISetupPtrs);
	SubscribeToMessage(MessageType::StartInGameAudio);
	SubscribeToMessage(MessageType::DialogueOver);

	myUsingFreeCamera = false;

	//River Entity init
	myRiverAudioEntity = myEntityPool.Retrieve();
	myRiverAudioEntity->Init(EntityType::None, myEntityID++);
	myEntitys.push_back(myRiverAudioEntity);
	myRiverAudioEntity->AddComponent<Audio>()->Init(myRiverAudioEntity);
	myRiverAudioEntity->GetComponent<Audio>()->InternalInit(AudioComponentType::River);
	//-----------------

	SetupPlayerAndCamera(V3F(0, 0, 0));
	myDynamicObjectFactory->SetPlayerPtr(myPlayer);
	myUIManager->Init(myPlayer, *myScenePtr, *aSpriteFactory, *myTextFactory, aCamera);

	myCinEditor.Init(aScene, myParticleFactory, aSpriteFactory);
}

void GameWorld::SetupPlayerAndCamera(CommonUtilities::Vector3<float> aSpawnPos)
{
	myPlayer = myEntityPool.Retrieve();
	myPlayer->Init(EntityType::Player, myEntityID++);
	myEntitys.push_back(myPlayer);
	myAIPollingStation->SetPlayer(myPlayer);

	myCamera = myEntityPool.Retrieve();
	myCamera->Init(EntityType::Camera, myEntityID++);
	myCamera->AddComponent<FollowCamera>()->Init(myCamera);
	myCamera->GetComponent<FollowCamera>()->SetTargetCamera(myMainCameraPtr);
	myCamera->GetComponent<FollowCamera>()->SetTargetEntity(myPlayer);

	myCamera->AddComponent<Audio>()->Init(myCamera);
	myCamera->GetComponent<Audio>()->InternalInit(AudioComponentType::Listener);

	SpawnPlayer();

	Entity* entity = myEntityPool.Retrieve();
	entity->Init(EntityType::TriggerBox, myEntityID++);
	entity->SetScale(V3F(1, 1, 1));
	entity->SetRotation(CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f));
	entity->Spawn(V3F(0, 0, 0));

	entity->AddComponent<Collision>()->Init(entity);
	entity->GetComponent<Collision>()->SetPlanevolume(
		{
			V3F(-1610,0,1079),
			V3F(-2444,0,-981),
			V3F(-2073,0,-1369),
			V3F(-955,0,-1663),
			V3F(2257,0,-877),
			V3F(2580,0,-590),
			V3F(2775,0,-186),
			V3F(2841,0,433),
			V3F(570,0,2684)
		}
	, V3F(0, 0, 0));
	entity->GetComponent<Collision>()->SetIsFriendly(false);

	entity->GetComponent<Collision>()->SetTriggerID(5000);
	entity->GetComponent<Collision>()->SetIsStrikeableTrigger(false);
	myTriggers.push_back(entity);
}

void GameWorld::ClearWorld(bool isShouldDeleteplayer)
{
	for (int index = CAST(int, myEntitys.size()) - 1; index >= 0; index--)
	{
		if (myEntitys[index]->GetEntityType() != EntityType::Player && myEntitys[index]->GetEntityType() != EntityType::Camera)
		{
			myEntitys[index]->Dispose();
			myEntityPool.Dispose(myEntitys[index]);
			myEntitys.erase(myEntitys.begin() + index);
		}
	}

	for (int index = CAST(int, myLights.size()) - 1; index >= 0; index--)
	{
		myLights[index]->Dispose();
		myEntityPool.Dispose(myLights[index]);
		myLights.erase(myLights.begin() + index);
	}

	for (int index = CAST(int, myTriggers.size()) - 1; index >= 0; index--)
	{
		myTriggers[index]->Dispose();
		myEntityPool.Dispose(myTriggers[index]);
		myTriggers.erase(myTriggers.begin() + index);
	}

	for (int index = CAST(int, myEnemies.size()) - 1; index >= 0; index--)
	{
		myEnemies[index]->Dispose();
		myEntityPool.Dispose(myEnemies[index]);
		myEnemies.erase(myEnemies.begin() + index);
	}

	for (int index = CAST(int, myPickups.size()) - 1; index >= 0; index--)
	{
		myPickups[index]->Dispose();
		myEntityPool.Dispose(myPickups[index]);
		myPickups.erase(myPickups.begin() + index);
	}

	for (int index = CAST(int, myAnimations.size()) - 1; index >= 0; index--)
	{
		myAnimations[index]->Dispose();
		myEntityPool.Dispose(myAnimations[index]);
		myAnimations.erase(myAnimations.begin() + index);
	}

	for (int index = myDialogs.size() - 1; index >= 0; index--)
	{
		myScenePtr->RemoveText(myDialogs[index]);
		delete myDialogs[index];
		myDialogs.erase(myDialogs.begin() + index);
	}

	if (isShouldDeleteplayer)
	{
		for (int index = CAST(int, myEntitys.size()) - 1; index >= 0; index--)
		{
			if (myEntitys[index]->GetEntityType() == EntityType::Player)
			{
				myEntitys[index]->Dispose();
				myEntityPool.Dispose(myEntitys[index]);
				myEntitys.erase(myEntitys.begin() + index);
			}
		}
	}
}

void GameWorld::SpawnPlayer()
{
	myPlayer->RemoveAllComponents();

	myPlayer->AddComponent<Mesh>()->Init(myPlayer);
	myPlayer->GetComponent<Mesh>()->SetUpModel("CH_player/CH_player.fbx");
	myPlayer->GetComponent<Mesh>()->SetScale(V3F(1.f, 1.f, 1.f));

	myPlayer->AddComponent<AnimationComponent>()->Init(myPlayer);
	myPlayer->SetIsMoving(true);

	myPlayer->AddComponent<Movement3D>()->Init(myPlayer);
	myPlayer->GetComponent<Movement3D>()->SetSpeed(100.0f);

	myPlayer->AddComponent<PlayerController>()->InternalInit(myGBPhysXPtr);
	myPlayer->GetComponent<PlayerController>()->SetFollowCameraPtr(myCamera->GetComponent<FollowCamera>());

	myPlayer->AddComponent<Collision>()->Init(myPlayer);
	myPlayer->GetComponent<Collision>()->SetCollisionRadius(50.0f);
	myPlayer->GetComponent<Collision>()->SetHeightOffset(35.0f);
	myPlayer->GetComponent<Collision>()->SetIsFriendly(true);

	myPlayer->AddComponent<Audio>()->Init(myPlayer);
	myPlayer->GetComponent<Audio>()->InternalInit(AudioComponentType::Player);

	Inventory* invent = myPlayer->AddComponent<Inventory>();
	invent->Init(myPlayer, myTextFactory, mySpriteFactory, myScenePtr);

	Item hoe = Item();
	hoe.myItemId = PreCalculatedItemIds::Hoe;
	invent->Add(hoe);

	Item wateringCan = Item();
	wateringCan.myItemId = PreCalculatedItemIds::WateringCanFilled;
	wateringCan.myCharges = 8;
	invent->Add(wateringCan);

	Item scissor = Item();
	scissor.myItemId = PreCalculatedItemIds::Scissor;
	invent->Add(scissor);

	Item basket = Item();
	basket.myItemId = PreCalculatedItemIds::Basket;
	invent->Add(basket);

	Item seeds = Item();
	seeds.myItemId = PreCalculatedItemIds::Seeds;
	seeds.myAmount = 10;
	invent->Add(seeds);

	myPlayer->Spawn({ 0, 0, 0 });
}

void GameWorld::TriggerEvent(const Message& anEvent)
{
	switch (anEvent.myIntValue)
	{

	case 6667:
	{
		SendMessages(MessageType::WinGameAndAlsoLife);
	}
	break;
	default:
		break;
	}
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

void GameWorld::CalculateRiverEmitterPos()
{
	V3F start;
	V3F middle;
	V3F end;

	std::array<V3F, 14> myRiverPoints;

	myRiverPoints[0] = V3F(4846, 10.f, -2889);
	myRiverPoints[1] = V3F(3824, 10.f, -1754);
	myRiverPoints[2] = V3F(2398, 10.f, -1204);
	myRiverPoints[3] = V3F(2300, 10.f, -1004);
	myRiverPoints[4] = V3F(2293, 10.f, -626);
	myRiverPoints[5] = V3F(2439, 10.f, -382);
	myRiverPoints[6] = V3F(2689, 10.f, -211);
	myRiverPoints[7] = V3F(3639, 10.f, 124);
	myRiverPoints[8] = V3F(4533, 10.f, 1864);
	myRiverPoints[9] = V3F(5460, 10.f, 2624);
	myRiverPoints[10] = V3F(6674, 10.f, 3160);
	myRiverPoints[11] = V3F(8314, 10.f, 5464);
	myRiverPoints[12] = V3F(10506, 10.f, 7264);
	myRiverPoints[13] = V3F(10796, 10.f, 8404);

	V3F pos;

	for (int i = 0; i < myRiverPoints.size() - 1; ++i)
	{
		pos = ClosestPointOnLine(myRiverPoints[i], myRiverPoints[i + 1], myPlayer->GetPosition());

		if (pos == myRiverPoints[i + 1])
		{
			continue;
		}
		else
		{
			break;
		}
	}

	for (int i = 0; i < myRiverPoints.size() - 1; ++i)
	{
		DebugDrawer::GetInstance().DrawLine(myRiverPoints[i], myRiverPoints[i + 1]);
	}

	DebugDrawer::GetInstance().DrawCross(pos, 10);

	myRiverAudioEntity->SetPosition(pos);

	if (myRiverMeshEntity)
	{
		myRiverMeshEntity->myInteractPoint = pos;
	}
}

void GameWorld::CreateSeed()
{
	for (int index = 0; index < 8; index++)
	{

		Entity* entity = myEntityPool.Retrieve();
		entity->Init(EntityType::EnvironmentInteractable, myEntityID++);

		entity->AddComponent<WildPlant>()->Init(entity);

		entity->AddComponent<Mesh>()->Init(entity);
		std::string modelName = entity->GetComponent<WildPlant>()->GetModelName();
		entity->GetComponent<Mesh>()->SetUpModel(modelName);

		entity->AddComponent<GBPhysXStaticComponent>()->Init(entity);
		entity->GetComponent<GBPhysXStaticComponent>()->SetFilePath(modelName);
		entity->GetComponent<GBPhysXStaticComponent>()->SetGBPhysXPtr(myGBPhysXPtr);

		entity->AddComponent<Audio>()->Init(entity);
		entity->GetComponent<Audio>()->InternalInit(AudioComponentType::GrowSpot);

		//TODO EMIL

		V3F randomSpawnPos = Tools::RandomRange(V3F(-5000.0f, 0.0f, -8000.0f), V3F(3000.0f, 0.0f, 8000.0f));

		while ((randomSpawnPos.x > -2000.0f && randomSpawnPos.x < 2000.0f && randomSpawnPos.z > -1200.0f && randomSpawnPos.z < 2000.0f) || PathFinder::GetInstance().Floorify(randomSpawnPos) == V3F())
		{
			randomSpawnPos = Tools::RandomRange(V3F(-5000.0f, 0.0f, -8000.0f), V3F(3000.0f, 0.0f, 8000.0f));
		}

		entity->Spawn(randomSpawnPos);
		entity->SetRotation(V3F());
		myPickups.push_back(entity);
	}
}

void GameWorld::RespawnPlayer(V3F& aPosition)
{
	//TODO CHECKPOINT POSITION?
	myPlayer->GetComponent<Mesh>()->SetTint({ 0, 0, 0, 1 });

	if (!myScenePtr->Contains(myPlayer->GetComponent<Mesh>()->GetModelInstance()))
	{
		myPlayer->GetComponent<Mesh>()->AddModelToScene();
	}

	myPlayer->GetComponent<PlayerController>()->SetInputBlock(false);

	myPlayer->SetPosition(V3F(aPosition.x, aPosition.y, aPosition.z));

	//GBPhysXKinematicComponent* component = myPlayer->GetComponent<GBPhysXKinematicComponent>();
	//component->Enable();
	//if (component->GetPhysXCharacter() == nullptr)
	//{
	//	component->AddGBPhysXCharacter(myGBPhysXPtr, myPlayer->GetPosition(), V3F(0.0f, 0.0f, 0.0f), 100.0f, 50.0f, true);
	//}
	//else
	//{
	//	component->GetPhysXCharacter()->Teleport(myPlayer->GetPosition());
	//	//TODO: add physx actor to physx scene
	//}
	myPlayer->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);

	myPlayer->GetComponent<Mesh>()->SetFading(false);
}

void GameWorld::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::NewLevelLoaded:
	{
		static bool first = true;
		if (!myHasDoneInitialSetup)
		{
			V3F pos = *(V3F*)aMessage.myData;
			myPlayer->SetPosition(pos);
			myPlayer->SetSpawnPos(pos);
			//myPlayer->GetComponent<GBPhysXKinematicComponent>()->Enable();
			//myPlayer->GetComponent<GBPhysXKinematicComponent>()->GetPhysXCharacter()->Teleport(myPlayer->GetPosition());
		}
		myHasDoneInitialSetup = true;

		myUIManager->AddUIToScene(*myScenePtr);

		/*if (aMessage.myIntValue2 == 9)
		{
			myPlayer->SetRotation(V3F(0.0f, TORAD(90), 0.0f));
		}*/

		SendMessages(MessageType::FadeIn);
	}
	break;
	case MessageType::EnemyDied:
	{

	}
	break;

	case MessageType::WindowResize:
	{
		myWindowSize.x = CAST(float, aMessage.myIntValue);
		myWindowSize.y = CAST(float, aMessage.myIntValue2);
	}
	break;

	case MessageType::TriggerEvent:
		TriggerEvent(aMessage);
		break;

	case MessageType::FadeInComplete:
	{
		myFirstFadeInComplete = true;
	}
	break;

	case MessageType::SpawnBulletHitParticle:
	{
		ParticleInstance* particle;
		BulletHitReport* rep = reinterpret_cast<BulletHitReport*>(aMessage.myData);

		if (aMessage.myBool) //Hit entity
		{
			particle = myParticleFactory->InstantiateParticle("BulletHitEnemy.part");
			particle->SetDirection(V4F(rep->normal, 0));
		}
		else
		{
			particle = myParticleFactory->InstantiateParticle("BulletHitObject.part");
			particle->SetDirection(V4F(rep->incomingAngle.Reflected(rep->normal), 0));
		}

		const float size = 10.f;

		particle->SetBounds(V4F(rep->position, 1) - V4F(1, 1, 1, 1) * size, V4F(rep->position, 1) + V4F(1, 1, 1, 1) * size);
		particle->RefreshTimeout(0.5f);
		myScenePtr->AddInstance(particle);

	}
	break;

	case MessageType::StartDialogue:
	{
		DialogInstance* dialog = myDialogFactory->CreateGameDialog(aMessage.myText.data(), V2F(40, 20));
		myDialogs.push_back(dialog);
		myScenePtr->AddText(dialog);
	}
	break;

	case MessageType::DialogueOver:

		for (size_t index = 0; index < myDialogs.size(); index++)
		{
			if (myDialogs[index]->ShouldBeRemoved())
			{
				myScenePtr->RemoveText(myDialogs[index]);
				delete myDialogs[index];
				myDialogs.erase(myDialogs.begin() + index);
				break;
			}
		}

		break;


	case MessageType::RespawnTrader:
	{
		RespawnTrader();
	}
	break;

	case MessageType::DespawnTrader:
	{
		if (myTrader != nullptr)
		{
			myTrader->SetIsAlive(false);
		}
	}
	break;

	case MessageType::RiverCreated:
	{
		myRiverMeshEntity = reinterpret_cast<Entity*>(aMessage.myData);
	}
	break;

	case MessageType::FadeOut:
	{
		myShouldUpdateDirectionalLight = false;
	}
	break;

	case MessageType::FadeOutComplete:
	{
		myShouldUpdateDirectionalLight = true;
	}
	break;

	case MessageType::CreateSeed:
	{
		CreateSeed();
	}
	break;

	case MessageType::RequestUISetupPtrs:
	{
		FoodCellar* foodCellar = CAST(FoodCellar*, aMessage.myData);
		foodCellar->SetUp(myScenePtr, myTextFactory, mySpriteFactory, myPlayer);
	}
	break;
	case MessageType::StartInGameAudio:
	{
		myRiverAudioEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::LoopStop);
		myRiverAudioEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::LoopPlay);
	}
	default:
		break;
	}
}

#if USEIMGUI
void GameWorld::ImGuiNode(ImGuiNodePackage& aPackage)
{
	if (ImGui::CollapsingHeader("Objects"))
	{
		for (auto& i : myEntitys)
		{
			i->ImGuiNode(aPackage);
		}
	}
}

#endif // !_RETAIL

void GameWorld::Update(CommonUtilities::InputHandler& aInputHandler, float aDeltatime)
{
	PerlinNoise noise;
	float now = Tools::GetTotalTime();
	WindSystem::GetInstance().SetBaseWind(V3F((noise.noise(now * 2, now * PI, 5) - 0.5) * 10000, 0, (noise.noise(now * 2, now * PI, 10) - 0.5) * 10000));

	CalculateRiverEmitterPos();

	if (myShouldUpdateDirectionalLight)
	{
		UpdateDirectionalLight(aDeltatime);
	}

#if USEIMGUI
	WindowControl::Window("Day cycle", [this, aDeltatime]()
		{
			const size_t count = 4;
			static std::array<V4F, count> colors;
			colors[0] = V4F(1.0f, 0.75294f, 0.6f, 1.0f);
			colors[1] = V4F(0.98f, 1.0f, 0.6666f, 1.0f);
			colors[2] = V4F(1.0f, 1.0f, 1.0f, 1.0f);
			colors[3] = V4F(0.3843137f, 0.345098f, 0.66666f, 1.0f);

			static std::array<V3F, count> directions;
			directions[0] = V3F(-0.826f, 0.234f, -0.512f);
			directions[1] = V3F(-0.457f, 0.732f, -0.489f);
			directions[2] = V3F(0.001f, 1.0f, 0.0f);
			directions[3] = V3F(0.773f, 0.451f, 0.446f);


			static float timeOfDay = TimeHandler::GetInstance().GetDayTimePercentage();
			bool somethingChanged = false;
			static bool animate = false;
			ImGui::Checkbox("Play", &animate);
			static float animationSpeed = 0.2f;
			ImGui::InputFloat("Animation speed", &animationSpeed);
			if (animate)
			{
				timeOfDay += animationSpeed * aDeltatime;
				if (timeOfDay > 1.f)
				{
					timeOfDay -= 1.f;
				}
				somethingChanged = true;
			}

			somethingChanged |= ImGui::SliderFloat("Time of day", &timeOfDay, 0.f, 1.f);
			ImGui::Text("Direction:");
			for (size_t i = 0; i < count; i++)
			{
				if (ImGui::DragFloat3(("C" + std::to_string(i + 1)).c_str(), &directions[i].x, 0.01f))
				{
					somethingChanged |= true;
					directions[i].Normalize();
				}
				DebugDrawer::GetInstance().DrawDirection(V3F(0, 0, 0), directions[i], 200.f);
			}

			{
				std::vector<V3F> points;
				for (float i = 0; i < 1.f; i += 0.02f)
				{
					points.push_back(Math::BezierInterpolation(directions, i) * 200.f);
				}
				DebugDrawer::GetInstance().DrawLines(points);
			}

			ImGui::Text("Color:");

			for (size_t i = 0; i < count; i++)
			{
				somethingChanged |= ImGui::ColorEdit4(("C" + std::to_string(i + 1)).c_str(), &colors[i].x);
			}

			if (somethingChanged)
			{
				EnvironmentLight* light = myScenePtr->GetEnvironmentLight();
				if (light)
				{
					light->myColor = Math::BezierInterpolation(colors, timeOfDay);
					light->myDirection = Math::BezierInterpolation(directions, timeOfDay);
				}
			}
		});
	WindowControl::Window("Items", []()
		{
			ItemImgui();
		});
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
							V3F pos = myScenePtr->GetMainCamera()->GetPosition();
							pos.Normalize();
							pos *= sphere.Radius() * 2;
							myScenePtr->GetMainCamera()->SetPosition(pos);
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
				if (ImGui::BeginTabItem("Skyboxes"))
				{
					ImGui::BeginChild("selection");
					if (DebugTools::FileList)
					{
						for (auto& file : DebugTools::FileList->operator[](".dds"))
						{
							size_t pos = file.find("skybox");
							if (pos == std::string::npos)
							{
								continue;
							}


							bool selected = (file == skyboxPath);
							if (ImGui::Selectable(file.c_str(), &selected))
							{
								ModelInstance* newBox = AssetManager::GetInstance().GetSkybox(file).InstansiateSkybox();
								if (newBox)
								{
									myScenePtr->SetSkybox(newBox);
									SAFE_DELETE(skybox);
									skybox = newBox;
									skyboxPath = file;
									if (DebugTools::myLightLoader)
									{
										EnvironmentLight* l = DebugTools::myLightLoader->LoadLight(file.substr(0, file.size() - 4) + "_light" + file.substr(file.size() - 4));
										if (l)
										{
											EnvironmentLight* old = myScenePtr->GetEnvironmentLight();
											if (old)
											{
												l->myColor = old->myColor;
												l->myDirection = old->myDirection;
												l->myIntensity = old->myIntensity;
												delete old;
											}

											myScenePtr->SetEnvironmentLight(l);
										}
									}
								}
								else
								{
									LOGWARNING("Could not load a skybox of: " + file);
								}
							}
						}
					}
					else
					{
						ImGui::Text("Not currently enabled, run in debug to enable.");
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
#endif // _DEBUG
		});
	bool particleEditorOpen = WindowControl::Window("Particle Editor", [&]()
		{
			myScenePtr->RefreshAll(1.f);
			myParticleFactory->EditParticles(myScenePtr);
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
#ifdef _DEBUG
			ImGui::Checkbox("Show Paths", &Movement3D::ourShowPaths);
#endif
			ImGui::Checkbox("Pause Game", &gameIsPaused);
		});

	if (particleEditorOpen != editParticles)
	{
		editParticles = particleEditorOpen;
		static V3F cameraPos;
		if (editParticles)
		{
			myScenePtr->Stash(Scene::StashOp::Push);
			cameraPos = myScenePtr->GetMainCamera()->GetPosition();
			myScenePtr->GetMainCamera()->SetPosition(myScenePtr->GetMainCamera()->GetForward() * -400.f);
		}
		else
		{
			myScenePtr->Stash(Scene::StashOp::Pop);
			myScenePtr->GetMainCamera()->SetPosition(cameraPos);
		}
	}
	if (modelViewerOpen != myIsInModelViewerMode)
	{
		myIsInModelViewerMode = modelViewerOpen;
		static V3F cameraPos;
		if (myIsInModelViewerMode)
		{
			myScenePtr->Stash(Scene::StashOp::Push);
			cameraPos = myScenePtr->GetMainCamera()->GetPosition();
			myScenePtr->GetMainCamera()->SetPosition(myScenePtr->GetMainCamera()->GetForward() * -400.f);
		}
		else
		{
			if (modelInstance)
			{
				myScenePtr->RemoveModel(modelInstance);
				delete modelInstance;
				modelInstance = nullptr;
			}
			SAFE_DELETE(animator);
			myScenePtr->Stash(Scene::StashOp::Pop);
			myScenePtr->GetMainCamera()->SetPosition(cameraPos);
		}
	}

	bool isRunningStandard = !gameIsPaused;
	if (myCinEditor.Run())
	{
		isRunningStandard = false;
	}



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
				myScenePtr->GetMainCamera()->SetPosition(myScenePtr->GetMainCamera()->GetPosition() * totalDiff);
			}
			else if (aInputHandler.IsMouseDown(CommonUtilities::InputHandler::Mouse::Mouse_Left))
			{

				V3F pos = myScenePtr->GetMainCamera()->GetPosition();
				float length = pos.Length();
				pos += myScenePtr->GetMainCamera()->GetRight() * length * -static_cast<float>(mp.x - lastmp.x) * 0.001f;
				pos += myScenePtr->GetMainCamera()->GetUp() * length * static_cast<float>(mp.y - lastmp.y) * 0.001f;
				pos = pos.GetNormalized() * length;

				myScenePtr->GetMainCamera()->SetPosition(pos);
				myScenePtr->GetMainCamera()->LookAt(V3F(0, 0, 0));
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
#ifdef  _DEBUG
		if (modelInstance && DebugTools::FileList)
		{
			static bool drawSkeleton = false;
			static bool drawSkeletonSpaces = false;
			if (ImGui::Begin("Model settings"))
			{
				ImGui::Checkbox("Draw Skeleton", &drawSkeleton);
				ImGui::Checkbox("Draw Skeleton spaces", &drawSkeletonSpaces);
				modelInstance->ImGuiNode(*DebugTools::FileList, myScenePtr->GetMainCamera());
				Model* model = modelInstance->GetModelAsset().GetAsModel();
				if (model)
				{
					ImGui::Separator();
					model->ImGuiNode(*DebugTools::FileList);
				}
			}
			ImGui::End();
			if (drawSkeleton)
			{
				DebugDrawer::GetInstance().DrawSkeleton(modelInstance);
			}
			if (drawSkeletonSpaces)
			{
				std::array<M44f, NUMBEROFANIMATIONBONES> spaces;
				auto positions = modelInstance->GetBonePositions();
				modelInstance->SetupanimationMatrixes(spaces);
				for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
				{
					DebugDrawer::GetInstance().DrawGizmo(positions[i], 15, spaces[i]);
				}
			}
		}
#endif
		static V3F lightOffset(-67, -34, 79);
		//ImGui::DragFloat3("Light arrow position", &lightOffset.x);
		if (myScenePtr->GetEnvironmentLight())
		{
			DebugDrawer::GetInstance().DrawDirection(
				myScenePtr->GetMainCamera()->GetPosition() +
				myScenePtr->GetMainCamera()->GetForward() * lightOffset.z +
				myScenePtr->GetMainCamera()->GetUp() * lightOffset.y +
				myScenePtr->GetMainCamera()->GetRight() * lightOffset.x,
				-myScenePtr->GetEnvironmentLight()->myDirection);
		}
	}
	else if (editParticles) {/*NO-OP*/ }
	else if (isRunningStandard)
#endif // !USEIMGUI
	{
#if USEIMGUI
		if (showBoundingBoxes)
		{
			CommonUtilities::AABB3D<float>* bb = myPlayer->GetComponent<Collision>()->GetBoxCollider();
			auto pos = bb->Min() + ((bb->Max() - bb->Min()) / 2.0f);
			DebugDrawer::GetInstance().DrawBoundingBox(*bb);
		}
#endif // USEIMGUI

		{
			EnvironmentLight* env = myScenePtr->GetEnvironmentLight();
			if (env)
			{
				env->myShadowCorePosition = myPlayer->GetPosition();
			}
		}
		// COMPONENTS UPDATE ---------------------------------------------------------------------------------------------

		{
			PERFORMANCETAG("Components");
			ComponentLake::GetInstance().UpdateComponents(aDeltatime);
		}

		// OCTREE UPDATE -------------------------------------------------------------------------------------------------

		if (myObjectTree != nullptr)
		{
			myObjectTree->Update(aDeltatime);
		}

		DebugDrawer::GetInstance().SetColor(V4F(1.0f, 0.0f, 0.0f, 1.0f));

		CommonUtilities::AABB3D<float>* playerBBox = myPlayer->GetComponent<Collision>()->GetBoxCollider();

		for (auto& trigger : myTriggers)
		{
			Collision* col = trigger->GetComponent<Collision>();
			if (col->IsAABB())
			{
				DebugDrawer::GetInstance().DrawBoundingBox(*col->GetBoxCollider());
			}

			V3F objectPos = col->GetCenter();

			objectPos.x = CLAMP(playerBBox->Min().x, playerBBox->Max().x, objectPos.x);
			objectPos.y = CLAMP(playerBBox->Min().y, playerBBox->Max().y, objectPos.y);
			objectPos.z = CLAMP(playerBBox->Min().z, playerBBox->Max().z, objectPos.z);

			if (col->IsInside(objectPos))
			{
				myPlayer->GetComponent<Collision>()->OnCollide(col);
				col->OnCollide(myPlayer->GetComponent<Collision>());
				break;
			}

		}

		//for (auto& pickup : myPickups)
		//{
		//	if (!pickup->GetComponent<Movement3D>()->GetIsLaunching())
		//	{
		//		Collision* col = pickup->GetComponent<Collision>();
		//		DebugDrawer::GetInstance().DrawBoundingBox(*col->GetBoxCollider());
		//
		//		if (myPlayer->GetComponent<Collision>()->GetBoxCollider()->IsInside(pickup->GetPosition()))
		//		{
		//			myPlayer->GetComponent<Collision>()->OnCollide(col);
		//			col->OnCollide(myPlayer->GetComponent<Collision>());
		//		}
		//	}
		//}
	}


	CommonUtilities::Vector3<float> movement = { 0.f, 0.f, 0.f };
	CommonUtilities::Vector3<float> rotation = { 0.f, 0.f, 0.f };

#ifndef _RETAIL
#if !DEMOSCENE
#if USEIMGUI
	if (myUsingFreeCamera || gameIsPaused)
#else
	if (myUsingFreeCamera)
#endif // !USEIMGUI
#endif
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
		myMainCameraPtr->Move(movement);
		myMainCameraPtr->Rotate(rotation);
	}
#if USEIMGUI
	else if (!isRunningStandard)
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

		V3F forward = myMainCameraPtr->GetForward();
		forward.y = 0;
		forward.Normalize();
		V3F right = forward.Cross(V3F(0, -1, 0));

		myMainCameraPtr->SetPosition(movement.z * forward + movement.x * right + movement.y * V3F(0, 1, 0) + myMainCameraPtr->GetPosition());
	}
#endif // USEIMGUI
#endif // !_RETAIL


	for (int index = CAST(int, myEntitys.size()) - 1; index >= 0; index--)
	{
		if (myEntitys[index]->GetShouldBeRemoved())
		{
			myEntitys[index]->Dispose();
			myEntityPool.Dispose(myEntitys[index]);
			myEntitys.erase(myEntitys.begin() + index);
		}
	}


	for (int index = CAST(int, myTriggers.size()) - 1; index >= 0; index--)
	{
		if (!myTriggers[index]->GetIsAlive())
		{
			myTriggers[index]->Dispose();
			myEntityPool.Dispose(myTriggers[index]);
			myTriggers.erase(myTriggers.begin() + index);
		}
	}

	for (int index = CAST(int, myEnemies.size()) - 1; index >= 0; index--)
	{
		if (myEnemies[index]->GetShouldBeRemoved())
		{
			if (myTrader == myEnemies[index])
			{
				myTrader = nullptr;
			}
			myEnemies[index]->Dispose();
			myEntityPool.Dispose(myEnemies[index]);
			myEnemies.erase(myEnemies.begin() + index);
		}
	}

	for (int index = CAST(int, myPickups.size()) - 1; index >= 0; index--)
	{
		if (!myPickups[index]->GetIsAlive())
		{
			myPickups[index]->Dispose();
			myEntityPool.Dispose(myPickups[index]);
			myPickups.erase(myPickups.begin() + index);
		}
	}

	for (int index = CAST(int, myAnimations.size()) - 1; index >= 0; index--)
	{
		if (myAnimations[index]->GetShouldBeRemoved())
		{
			myAnimations[index]->Dispose();
			myEntityPool.Dispose(myAnimations[index]);
			myAnimations.erase(myAnimations.begin() + index);
		}
	}

	myUIManager->Update(aDeltatime);
	TimeHandler::GetInstance().Update(aDeltatime);
}

void GameWorld::UpdateDirectionalLight(float aDeltatime)
{
	const size_t count = 4;
	static std::array<V4F, count> colors;
	colors[0] = V4F(1.0f, 0.75294f, 0.6f, 1.0f);
	colors[1] = V4F(0.98f, 1.0f, 0.6666f, 1.0f);
	colors[2] = V4F(1.0f, 1.0f, 1.0f, 1.0f);
	colors[3] = V4F(0.3843137f, 0.345098f, 0.66666f, 1.0f);

	static std::array<V3F, count> directions;
	directions[0] = V3F(-0.826f, 0.234f, -0.512f);
	directions[1] = V3F(-0.457f, 0.732f, -0.489f);
	directions[2] = V3F(0.001f, 1.0f, 0.0f);
	directions[3] = V3F(0.773f, 0.451f, 0.446f);

	float timeOfDay = TimeHandler::GetInstance().GetDayTimePercentage();

	EnvironmentLight* light = myScenePtr->GetEnvironmentLight();
	if (light)
	{
		light->myColor = Math::BezierInterpolation(colors, timeOfDay);
		light->myDirection = Math::BezierInterpolation(directions, timeOfDay);
	}
}

void GameWorld::RespawnTrader()
{
	Message message;
	message.myMessageType = MessageType::SendUIGameMessage;
	message.myText = "The trader has arrived!";
	SendMessages(message);
}