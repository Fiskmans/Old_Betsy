#pragma once
#include "Component.h"
class ChickenCoop : public Component, public Observer, public Publisher
{
public:
	virtual void InteractWith(Item* aHeldItem);

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	bool HasEggs();

private:

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;

	size_t myEggAmount = 3;
	size_t myChickenAmount = 3;
};

