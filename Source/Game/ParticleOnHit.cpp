#include "pch.h"
#include "ParticleOnHit.h"
#include "Scene.h"
#include "ParticleFactory.h"
#include "ParticleInstance.h"
#include "Entity.h"

ParticlesOnHit::ParticlesOnHit()
{
	myScene = nullptr;
	myFactory = nullptr;
	myEntity = nullptr;

	mySize = 0.f;

	myPositionOffset = V3F(0, 0, 0);

	myParticleType = "";
}

ParticlesOnHit::~ParticlesOnHit()
{
	WIPE(*this);
}

void ParticlesOnHit::PreInit(Scene* aScene, ParticleFactory* aFactory)
{
	myFactory = aFactory;
	myScene = aScene;
}

void ParticlesOnHit::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void ParticlesOnHit::Update(const float aDeltaTime)
{
}

void ParticlesOnHit::Reset()
{
}

void ParticlesOnHit::SetParticle(const std::string& aParticle, float aSize, const V3F& aPositionOffset)
{
	myParticleType = aParticle;
	mySize = aSize;
	myPositionOffset = aPositionOffset;
}

void ParticlesOnHit::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::WasHit)
	{
		ParticleInstance* inst = myFactory->InstantiateParticle(myParticleType);
		if (inst)
		{
			V4F pos;
			if (someData != nullptr)
			{
				pos = V4F(*CAST(V3F*, someData), 1.f);
			}
			else
			{
				pos = V4F(myEntity->GetPosition() + myPositionOffset, 1);
			}

			inst->SetBounds(pos - V4F(1, 1, 1, 0) * mySize, pos - V4F(1, 1, 1, 0) * mySize);
			inst->RefreshTimeout(0.5f);

			myScene->AddInstance(inst);
		}
	}
}

void ParticlesOnHit::OnAttach()
{
}

void ParticlesOnHit::OnDetach()
{
}

void ParticlesOnHit::OnKillMe()
{
}
