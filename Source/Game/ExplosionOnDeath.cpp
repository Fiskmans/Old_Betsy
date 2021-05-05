#include "pch.pch"
#include "ExplosionOnDeath.h"
#include "Scene.h"
#include "ParticleFactory.h"
#include "ParticleInstance.h"
#include "Entity.h"

ExplosionOnDeath::ExplosionOnDeath()
{
	myComponentType = ComponentType::Explosion;
}

void ExplosionOnDeath::PreInit(CScene* aScene, ParticleFactory* aFactory)
{
	myFactory = aFactory;
	myScene = aScene;
}

void ExplosionOnDeath::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void ExplosionOnDeath::Update(const float aDeltaTime)
{
}

void ExplosionOnDeath::Reset()
{
	ParticleInstance* inst = myFactory->InstantiateParticle("Explosion.part");
	if (inst)
	{
		V4F pos = V4F(myEntity->GetPosition(), 1);
		inst->SetBounds(pos - V4F(1, 1, 1, 0) * 5.f, pos - V4F(1, 1, 1, 0) * 5.f);
		myScene->AddInstance(inst);
	}
}
