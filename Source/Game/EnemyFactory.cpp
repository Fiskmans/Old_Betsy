#include "pch.h"
#include "EnemyFactory.h"
#include "Entity.h"
#include "DataStructs.h"
#include "ObjectPool.hpp"
#include "ComponentLake.h"
#include "Mesh.h"
#include "Collision.h"
#include "Life.h"
#include "Audio.h"
#include "ParticleOnDeath.h"
#include "AnimationComponent.h"
#include "AIController.h"
#include "Movement3D.h"
#include "CharacterInstance.h"
#include "ParticleOnHit.h"
#include "AABB3D.hpp"
#include "Octree.h"
#include "AIPollingStation.h"
#include "GBPhysXComponent.h"
#include <Random.h>
#include "GBPhysXKinematicComponent.h"
#include  "NodePollingStation.h"
#include "AIBaseState.h"
#include "TraderAI.h"
#include "ParticleActivatable.h"

EnemyFactory::EnemyFactory() :
	myComponentLake(nullptr),
	myOctree(nullptr),
	myEntityPoolPtr(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr),
	myEnemyTarget(nullptr),
	myCharacterData(nullptr),
	myAIPollingStation(nullptr),
	myMayaPosPollingStationPtr(nullptr)
{
}


EnemyFactory::~EnemyFactory()
{
	UnSubscribeToMessage(MessageType::SpawnEnemy);
	UnSubscribeToMessage(MessageType::NewOctreeCreated);
	UnSubscribeToMessage(MessageType::PlayerDied);
	UnSubscribeToMessage(MessageType::StartLoading);
	UnSubscribeToMessage(MessageType::UnloadLevel);
}

void EnemyFactory::Init(AIPollingStation* aAIPollingStation, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, CharacterData* aCharData, GBPhysX* aGBPhysXPtr)
{
	SubscribeToMessage(MessageType::SpawnEnemy);
	SubscribeToMessage(MessageType::NewOctreeCreated);
	SubscribeToMessage(MessageType::PlayerDied);
	SubscribeToMessage(MessageType::StartLoading);
	SubscribeToMessage(MessageType::UnloadLevel);

	myOctree = aOctTree;
	myEntityVector = aEntityVector;
	myEntityPoolPtr = aEntityPool;
	myEntityIDInt = aEntityIDInt;
	myComponentLake = aComponentLake;
	myCharacterData = aCharData;
	myGBPhysX = aGBPhysXPtr;

	myAIPollingStation = aAIPollingStation;
}

Entity* EnemyFactory::CreateEnemy(const std::string& aFilePath, int aEnemyID, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, int aTargetPosID, int aAnimationID, int aTriggerVolumeID, bool aIsSpawnInRuntime)
{
	Entity* entity = myEntityPoolPtr->Retrieve();
	entity->Init(EntityType::Enemy, *myEntityIDInt);
	*myEntityIDInt = *myEntityIDInt + 1;

	aRot.x = 0;
	aRot.z = 0;

	int CharacterTypeID = aEnemyID;

	entity->AddComponent<CharacterInstance>()->Init(entity, myCharacterData->GetCharacterStats(CharacterTypeID));

	Stats* stats = (entity->GetComponent<CharacterInstance>()->GetCharacterStats());

	entity->AddComponent<Mesh>()->Init(entity);
	entity->GetComponent<Mesh>()->SetUpModel(stats->fbxPath);

	entity->GetComponent<Mesh>()->SetScale({ stats->modelScale, stats->modelScale, stats->modelScale });
	entity->GetComponent<Mesh>()->SetShouldBeDrawnThroughWalls(false);


	entity->AddComponent<AnimationComponent>()->Init(entity);

	entity->SetRotation(aRot);

	entity->AddComponent<Collision>()->Init(entity);
	entity->GetComponent<Collision>()->SetCollisionRadius(stats->collisionRadius);
	entity->SetIsMoving(true);
	entity->GetComponent<Collision>()->SetHeightOffset(stats->height * 0.5f);
	entity->GetComponent<Collision>()->SetIsFriendly(false);

	entity->AddComponent<Life>()->Init(entity, stats->life);

	if (CharacterTypeID != 3)
	{
		entity->AddComponent<AIController>()->Init(entity);


		V3F* pos = myMayaPosPollingStationPtr->GetMayaPos(aTargetPosID);
		if (pos)
		{
			entity->GetComponent<AIController>()->SetTargetPosition(*pos);
		}
	}

	entity->AddComponent<Audio>()->Init(entity);

	if (CharacterTypeID == 1)
	{
		entity->GetComponent<Audio>()->InternalInit(AudioComponentType::Hen);
	}
	else if (CharacterTypeID == 2)
	{
		entity->GetComponent<Audio>()->InternalInit(AudioComponentType::Sheep);
		entity->AddComponent<ParticleActivatable>()->Init(entity);
		entity->GetComponent<ParticleActivatable>()->SetParticle("SheepCut.part", 10, {0,80,0});
	}
	else if (CharacterTypeID == 3)
	{
		entity->GetComponent<Audio>()->InternalInit(AudioComponentType::Trader);
	}

	entity->AddComponent<ParticlesOnDeath>()->Init(entity);
	entity->GetComponent<ParticlesOnDeath>()->SetParticle("EnemyDeath.part", 10.f, { 0.f, 100.f, 0.f });

	entity->AddComponent<ParticlesOnHit>()->Init(entity);
	entity->GetComponent<ParticlesOnHit>()->SetParticle("EnemyHit.part", 10.f, { 0.f, 100.f, 0.f });

	entity->AddComponent<GBPhysXKinematicComponent>()->Init(entity);
	V3F offset = V3F(0.0f, 200.0f, 0.0f);

	if (CharacterTypeID != 3)
	{
		entity->GetComponent<AIController>()->AIInit(myAIPollingStation, AIStates::Idle, stats->characterID, aIsSpawnInRuntime);
	}
	else
	{
		entity->AddComponent<TraderAI>()->Init(entity);
		entity->GetComponent<TraderAI>()->AIInit(myAIPollingStation, TraderState::None);
	}

	if (aIsSpawnInRuntime)
	{
		V3F setPos;

		V3F rayStart = aPos;
		rayStart.y += 800;
		V3F rayDir = aPos - rayStart;
		rayDir.Normalize();

		SlabRay ray(rayStart, rayDir);
		setPos = PathFinder::GetInstance().FindPoint(ray);

		if (setPos.LengthSqr() > 0)
		{
			entity->Spawn(setPos + V3F(0, 30, 0) + offset);
		}
		else
		{
			entity->Spawn(aPos + offset);
		}
	}
	else
	{
		entity->Spawn(aPos + offset);
	}

	entity->GetComponent<GBPhysXKinematicComponent>()->AddGBPhysXCharacter(myGBPhysX, entity->GetPosition(), aRot, 100.0f, 50.0f, false);

	myEntityVector->push_back(entity);
	if (myCurrentLevel)
	{
		myCurrentLevel->push_back(entity);
	}
	else
	{
		SYSWARNING("Loading enemy outside of level streaming, it will not be cleaned up properly", aFilePath);
	}

	if (myOctree != nullptr)
	{
		if (myOctree->ourTreeBuilt)
		{
			myOctree->ourPendingInsertionList.push(entity->GetComponent<Collision>());
		}
	}

	return entity;
}

void EnemyFactory::SpawnQueued(const float aDeltaTime)
{
	if (myQueue.size() <= 0)
	{
		return;
	}

	Entity* enemy;
	V3F dir;

	for (int i = myQueue.size() - 1; i >= 0; i--)
	{
		myQueue[i].currentTimeInterval -= aDeltaTime;

		if (myQueue[i].currentTimeInterval <= 0.f && myQueue[i].numberOfEnemiesSpawned < myQueue[i].numberOfEnemiesToSpawn)
		{
			dir = Tools::RandomDirection();
			enemy = CreateEnemy("", myQueue[i].typeID, myQueue[i].position + (dir * Tools::RandomRange(-myQueue[i].radius, myQueue[i].radius) * V3F(1.f, 0.f, 1.f)), Tools::RandomDirection(), V3F(1.0f, 1.0f, 1.0f), -1, 1, -1, true);

			if (myQueue[i].spawnFalling)
			{
				enemy->SendEntityMessage(EntityMessage::StartFalling);
			}
			else if (myQueue[i].spawnSleeping)
			{
				//PYRO sleep here 
			}

			if (!myQueue[i].targetPlayer && myQueue[i].targetPos != V3F(0, 0, 0))
			{
				dir = Tools::RandomDirection();
				enemy->GetComponent<AIController>()->SetTargetPosition(myQueue[i].targetPos + (dir * Tools::RandomRange(-myQueue[i].targetRadius, myQueue[i].targetRadius) * V3F(1.f, 0.f, 1.f)));
			}
			else
			{
				enemy->GetComponent<AIController>()->SetTargetEntity(myAIPollingStation->GetPlayer());
			}

			myQueue[i].numberOfEnemiesSpawned++;

			SYSINFO("Enemy " + std::to_string(myQueue[i].numberOfEnemiesSpawned) + " out of " + std::to_string(myQueue[i].numberOfEnemiesToSpawn) + " spawned at " + std::to_string(enemy->GetPosition().x) + " " + std::to_string(enemy->GetPosition().y) + " " + std::to_string(enemy->GetPosition().z));

			myQueue[i].currentTimeInterval = Tools::RandomRange(myQueue[i].timeIntervalMin, myQueue[i].timeIntervalMax);

			if (myQueue[i].contniuous)
			{
				myQueue[i].spawnedEnemies.push_back(enemy);
			}
			else if (myQueue[i].numberOfEnemiesSpawned >= myQueue[i].numberOfEnemiesToSpawn)
			{
				SYSINFO("All enemies spawned, removing from queue");
				myQueue.erase(myQueue.begin() + i);
				continue;
			}
		}

		if (myQueue[i].contniuous)
		{
			if (!(*myQueue[i].isActive))
			{
				SYSINFO("Continuous spawner stopped");

				delete myQueue[i].isActive;
				myQueue[i].spawnedEnemies.clear();
				myQueue.erase(myQueue.begin() + i);
				continue;
			}

			for (int j = myQueue[i].spawnedEnemies.size() - 1; j >= 0; --j)
			{
				if (!myQueue[i].spawnedEnemies[j]->GetIsAlive())
				{
					myQueue[i].spawnedEnemies.erase(myQueue[i].spawnedEnemies.begin() + j);
					myQueue[i].numberOfEnemiesSpawned--;

					SYSINFO("Removing continously spawned enemy, " + std::to_string(myQueue[i].numberOfEnemiesSpawned) + " out of " + std::to_string(myQueue[i].numberOfEnemiesToSpawn) + " left")
				}
			}
		}
	}
}

void EnemyFactory::QueueSpawn(int aCharID, int aNumberOfEnemiesToSpawn, float aTimeIntervalMin, float aTimeIntervalMax, const V3F& aPosition, float aRadius, const V3F& aTargetPos, float aTargetRadius, bool aTargetPlayer, bool aSpawnFalling, bool aIsContinuous, bool aSpawnSleeping)
{
	QueuedSpawn spawn;

	spawn.typeID = aCharID;
	spawn.numberOfEnemiesToSpawn = aNumberOfEnemiesToSpawn;
	spawn.numberOfEnemiesSpawned = 0;
	spawn.timeIntervalMin = aTimeIntervalMin;
	spawn.timeIntervalMax = aTimeIntervalMax;
	spawn.currentTimeInterval = Tools::RandomRange(spawn.timeIntervalMin, spawn.timeIntervalMax);
	spawn.position = aPosition;
	spawn.radius = aRadius;
	spawn.targetPos = aTargetPos;
	spawn.targetRadius = aTargetRadius;
	spawn.targetPlayer = aTargetPlayer;
	spawn.spawnFalling = aSpawnFalling;
	spawn.contniuous = aIsContinuous;
	spawn.isActive = nullptr;

	SYSINFO((spawn.contniuous == true ? "Contniuous spawner started" : "Spawner started"));

	myQueue.push_back(spawn);
}

void EnemyFactory::QueueSpawn(bool*& aIsActiveOut, int aCharID, int aNumberOfEnemiesToSpawn, float timeIntervalMin, float timeIntervalMax, const V3F& aPosition, float aRadius, const V3F& aTargetPos, float aTargetRadius, bool aTargetPlayer, bool aSpawnFalling, bool aIsContinuous)
{
	QueueSpawn(aCharID, aNumberOfEnemiesToSpawn, timeIntervalMin, timeIntervalMax, aPosition, aRadius, aTargetPos, aTargetRadius, aTargetPlayer, aSpawnFalling, aIsContinuous);
	myQueue.back().isActive = new bool(true);

	aIsActiveOut = myQueue.back().isActive;
}

void EnemyFactory::ClearQueue()
{
	myQueue.clear();
}

void EnemyFactory::SetPollingStation(NodePollingStation* aPollingStation)
{
	myMayaPosPollingStationPtr = aPollingStation;
}


float EnemyFactory::GetFloatMetricValueFromLine(const std::string& aLine)
{
	bool valueIsNext = false;
	std::string returnValue;

	for (auto symbol : aLine)
	{
		if (valueIsNext)
		{
			returnValue += symbol;
		}

		if (symbol == *":")
		{
			valueIsNext = true;
		}
	}
	return CAST(float, atof(returnValue.c_str()));
}

int EnemyFactory::GetIntMetricValueFromLine(const std::string& aLine)
{
	bool valueIsNext = false;
	std::string returnValue;

	for (auto symbol : aLine)
	{
		if (valueIsNext)
		{
			returnValue += symbol;
		}

		if (symbol == *":")
		{
			valueIsNext = true;
		}
	}
	return atoi(returnValue.c_str());
}

void EnemyFactory::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::SpawnEnemy)
	{
		EnemyInstanceBuffer* buffer = static_cast<EnemyInstanceBuffer*>(aMessage.myData);
		int enemyID = buffer->enemyID;
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> rot = { buffer->rotation[0],buffer->rotation[1], buffer->rotation[2] };
		CommonUtilities::Vector3<float> scale = { buffer->scale[0],buffer->scale[1], buffer->scale[2] };

		CreateEnemy(buffer->aFilePath, enemyID, pos, rot, scale, -1, buffer->animationID, -1, false);

	}
	else if (aMessage.myMessageType == MessageType::NewOctreeCreated)
	{
		myOctree = (Octree*)aMessage.myData;
	}
	else if (aMessage.myMessageType == MessageType::PlayerDied)
	{
		myAIPollingStation->ClearSeekingEnemies();
	}


	switch (aMessage.myMessageType)
	{
	case MessageType::StartLoading:
	{
		myCurrentLevel = new std::vector<Entity*>();
		myLevelObjectMapping[aMessage.myText.data()] = myCurrentLevel;
	}
	break;
	case MessageType::UnloadLevel:
	{
		if (myLevelObjectMapping.count(aMessage.myText.data()) != 0)
		{
			auto& toRemove = *myLevelObjectMapping[aMessage.myText.data()];
			for (auto& ent : toRemove)
			{
				for (int i = int(myEntityVector->size()) - 1; i >= 0; --i)
				{
					if (myEntityVector->operator[](i) == ent)
					{
						ent->Dispose();
						myEntityPoolPtr->Dispose(ent);
						myEntityVector->erase(myEntityVector->begin() + i);
					}
				}
			}
			delete myLevelObjectMapping[aMessage.myText.data()];
			myLevelObjectMapping.erase(aMessage.myText.data());
		}
		else
		{
			SYSWARNING("Trying to unload level thats not loaded", aMessage.myText.data());
		}
	}
	break;
	}
}

