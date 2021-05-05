#pragma once
#include "Component.h"
class LifeTime :
	public Component
{
public:
	LifeTime();
	virtual ~LifeTime();

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetLifeTime(const float aTime);

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	float myCurrentLifeTime;
	float mySetLifeTime;
};

