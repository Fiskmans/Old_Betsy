#pragma once
#include "Component.h"

class WildPlant : public Component, public Publisher
{

public:
	
	WildPlant();
	~WildPlant();

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	virtual void InteractWith(Item* aHeldItem) override;
	void SetPlantTypeID(int aTypeID);
	int GetPlantTypeID();
	void SetPlayerPtr(Entity* aPlayerPtr);
	std::string GetModelName();

private:

	float myPickPlantTimer = 0;
	bool myStartTimer = false;

	int myPlantTypeID;
	std::string myPlantModelName;
	ItemCreationStruct myItemData;
	std::unordered_map<ItemIdType, std::string> mySeedMap;
	std::vector<int> mySeedIds;
	Entity* myPlayerPtr;
};

