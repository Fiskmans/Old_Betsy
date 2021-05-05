#include "pch.h"
#include "ParticleOnDeath.h"
#include "Scene.h"
#include "ParticleFactory.h"
#include "ParticleInstance.h"
#include "Entity.h"

ParticlesOnDeath::ParticlesOnDeath()
{
}

void ParticlesOnDeath::PreInit(Scene* aScene, ParticleFactory* aFactory)
{
	myFactory = aFactory;
	myScene = aScene;
}

void ParticlesOnDeath::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void ParticlesOnDeath::Update(const float aDeltaTime)
{
}

void ParticlesOnDeath::Reset()
{
}

void ParticlesOnDeath::SetParticle(std::string aParticle, float aSize, V3F aOffset)
{
	myParticleType = aParticle;
	myOffset = aOffset;
	mySize = aSize;
}

void ParticlesOnDeath::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::StartDying)
	{
		ParticleInstance* inst = myFactory->InstantiateParticle(myParticleType);
		if (inst)
		{
			V4F pos = V4F(myEntity->GetPosition() + myOffset, 1);
			inst->SetBounds(pos - V4F(1, 1, 1, 0) * mySize, pos - V4F(1, 1, 1, 0) * mySize);
			myScene->AddInstance(inst);
			inst->RefreshTimeout(0.5f);
		}
	}
}

void ParticlesOnDeath::OnAttach()
{
}

void ParticlesOnDeath::OnDetach()
{
}

void ParticlesOnDeath::OnKillMe()
{
}
