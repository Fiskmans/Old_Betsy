#pragma once
#include "Observer.hpp"
#include "Vector3.hpp"
#include <vector>
#include "ObjectPoolSizes.h"
#include "Spline.h"
#include <imgui.h>
#include "MetricValue.h"
#include "EmilsEnums.h"
#ifdef __INTELLISENSE__
#include <pch.h>
#endif

namespace CommonUtilities
{
	template <typename Type>
	class ObjectPool;
}
class Octree;
class Entity;
class ComponentLake;
class CharacterData;
class AbilityData;
class AIPollingStation;
class GBPhysX;
class NodePollingStation;

struct EnemyMetrics
{
	EnemyType myEnemyType = EnemyType::None;
	unsigned int myHP = 1;
	float mySpeed = 0.0f;
	float myWeaponCooldown = 0.0f;
	float myCollisionRadius = 1.0f;
	float myProjectileDamage = 0.0f;
	float myProjectileSpeed = 0.0f;
};

class EnemyFactory : public Observer
{
public:

	EnemyFactory();
	~EnemyFactory();

	void Init(AIPollingStation* aAIPollingStation, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, CharacterData* aCharData, GBPhysX* aGBPhysXPtr);
	Entity* CreateEnemy(const std::string& aFilePath, int aEnemyID, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, int aTargetPosID = -1, int aAnimationID = 0, int aTriggerVolumeID = -1, bool aIsSpawnedInRuntime = false);

	void SpawnQueued(const float aDeltaTime);

	void QueueSpawn(int aCharID, int aNumberOfEnemiesToSpawn, float timeIntervalMin, float timeIntervalMax, const V3F& aPosition, float aRadius, const V3F& aTargetPos, float aTargetRadius, bool aTargetPlayer = false, bool aSpawnFalling = false, bool aIsContinuous = false, bool aSpawnSleeping = false);
	void QueueSpawn(bool*& aIsActiveOut, int aCharID, int aNumberOfEnemiesToSpawn, float timeIntervalMin, float timeIntervalMax, const V3F& aPosition, float aRadius, const V3F& aTargetPos, float aTargetRadius, bool aTargetPlayer = false, bool aSpawnFalling = false, bool aIsContinuous = false);

	void ClearQueue();

	void SetPollingStation(NodePollingStation* aPollingStation);

private:
	CharacterData* myCharacterData;

	float GetFloatMetricValueFromLine(const std::string& aLine);
	int GetIntMetricValueFromLine(const std::string& aLine);

	virtual void RecieveMessage(const Message& aMessage) override;

	Octree* myOctree;
	Entity* myEnemyTarget;
	std::vector<Entity*>* myEntityVector;
	CommonUtilities::ObjectPool<Entity>* myEntityPoolPtr;
	unsigned int* myEntityIDInt;
	ComponentLake* myComponentLake;
	GBPhysX* myGBPhysX = nullptr;
	NodePollingStation* myMayaPosPollingStationPtr;

	std::map<std::string, std::vector<Entity*>*> myLevelObjectMapping;
	std::vector<Entity*>* myCurrentLevel;

	AIPollingStation* myAIPollingStation;

	struct QueuedSpawn
	{
		int typeID;
		int numberOfEnemiesToSpawn;
		int numberOfEnemiesSpawned;
		float timeIntervalMin;
		float timeIntervalMax;
		float currentTimeInterval;
		V3F position;
		float radius;
		V3F targetPos;
		float targetRadius;
		bool targetPlayer;
		bool spawnFalling;
		bool spawnSleeping;
		bool contniuous;
		bool* isActive;

		std::vector<Entity*> spawnedEnemies;
	};

	std::vector<QueuedSpawn> myQueue;
};

