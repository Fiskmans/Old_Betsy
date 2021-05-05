#pragma once
#include "Component.h"
class CScene;
class ParticleFactory;
class ExplosionOnDeath :
	public Component
{
public:
	ExplosionOnDeath();

	void PreInit(CScene* aScene, ParticleFactory* aFactory);

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

private:
	CScene* myScene;
	ParticleFactory* myFactory;
};

