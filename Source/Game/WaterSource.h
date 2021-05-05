#pragma once
#include "Component.h"
class WaterSource : public Component, public Publisher
{
public:
	virtual void InteractWith(Item* aHeldItem) override;
	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

private:
	GAMEMETRIC(int, myWaterCanSize, PLAYERWATERINGCANSIZE,8);
};

