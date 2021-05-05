#pragma once
#include "Component.h"

class PlantInstance;
class Item;

class Growable : public Component, public Observer
{

public:
	~Growable();

	void SetPlant(const std::string& aPlantFile, Entity* aGroundEntity);

	virtual void InteractWith(Item* aHeldItem) override;
	void Spawn();
	bool Water();
	void Harvest();

	PlantInstance* GetPlantInstance();

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;


	virtual void SubscribeToMyMessages() final override;
	virtual void UnSubscribeToMyMessages() final override;

private:
	Entity* myGroundEntity;
	PlantInstance* myPlant;

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
};

