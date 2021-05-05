#pragma once
#include "Component.h"
class Loot : public Component
{

public:
	Loot();
	~Loot();

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;
};

