#pragma once
#include "Component.h"
#include "GBPhysX\GBPhysX.h"

class GrowthSpot : public Component, public Observer, public Publisher
{
public:

	enum class GrowthSpotEvent
	{
		HoeHitsGround,
		PlantIsPlaces,
		GroundGetsWet,
		HarvestingPlant,
	};

	void PreInit(std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aPool);
	void SetGBPhysXPointer(GBPhysX* aGBPhysX);

	Entity* myPlant;

	void Spawn();
	void RemovePlant();

	virtual void InteractWith(Item* aHeldItem) override;

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

	virtual void SubscribeToMyMessages() final override;
	virtual void UnSubscribeToMyMessages() final override;

	bool Water();
	void Dry();
	void Plow();

	bool GetIsMOIST();
	bool GetIsHoed();

	static void PopulateSeeds();
	static bool CanBePlanted(int aItemID);

	static const std::unordered_map<ItemIdType, std::string>& GetSeedMap();

private:

	struct QueuedEvent
	{
		float timer = 0;
		GrowthSpotEvent event;
		Item* item = nullptr;
		int plantID = -1;
	};

	std::vector<QueuedEvent> myQueuedEvents;

	static std::unordered_map<ItemIdType, std::string> ourSeedMapping;

	void SpawnPlant(const std::string& aPlantPath);

	GBPhysX* myGBPhysX;

	bool myIsWatered;
	bool myIsTilled;

	std::vector<Entity*>* myEntityVector;
	CommonUtilities::ObjectPool<Entity>* myEntityPool;

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
	std::string GetRandomDirt();
};

