#include "pch.h"
#include "ParticleActivatable.h"
#include "Scene.h"
#include "ParticleFactory.h"
#include "ParticleInstance.h"
#include "Entity.h"

ParticleActivatable::ParticleActivatable()
{
	myScene = nullptr;
	myFactory = nullptr;
	myEntity = nullptr;
}

ParticleActivatable::~ParticleActivatable()
{
	WIPE(*this);
}

void ParticleActivatable::PreInit(Scene* aScene, ParticleFactory* aFactory)
{
	myFactory = aFactory;
	myScene = aScene;
}

void ParticleActivatable::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void ParticleActivatable::Update(const float aDeltaTime)
{
}

void ParticleActivatable::Reset()
{
}

void ParticleActivatable::SetParticle(const std::string& aParticle, float aSize, const V3F& aPositionOffset)
{
	SParticle particle;
	particle.myParticleType = aParticle;
	particle.mySize = aSize;
	particle.myPositionOffset = aPositionOffset;
	myParticles.push_back(particle);
}

void ParticleActivatable::SetOffset(V3F anOffset)
{
	for (auto& particle : myParticles)
	{
		particle.myPositionOffset = anOffset;
	}
}

void ParticleActivatable::ActivateParticle(float aDuration)
{
	for (auto particle : myParticles)
	{
		ParticleInstance* inst = myFactory->InstantiateParticle(particle.myParticleType);
		if (inst)
		{
			V4F pos = V4F(myEntity->GetPosition() + particle.myPositionOffset, 1);
			inst->SetBounds(pos - V4F(1, 1, 1, 0) * particle.mySize, pos - V4F(1, 1, 1, 0) * particle.mySize);
			inst->RefreshTimeout(aDuration);

			myScene->AddInstance(inst);
		}
	}
}

void ParticleActivatable::OnAttach()
{
}

void ParticleActivatable::OnDetach()
{
}

void ParticleActivatable::OnKillMe()
{
}