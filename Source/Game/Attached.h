#pragma once
#include "Component.h"
class Attached :
	public Component
{
	void SetupAttachement(Entity* aEntity, int aBoneIndex);

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;
public:
	Entity* myEntityAttachedTo = nullptr;
	int myAttachedBoneIndex = -1;
};

