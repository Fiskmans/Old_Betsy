#pragma once
#include "pch.h"
#include "ParticlesWhileAlive.h"
#include "ParticleFactory.h"
#include "ParticleInstance.h"
#include <Scene.h>
#include "Entity.h"
#include "Mesh.h"

void ParticlesWhileAlive::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void ParticlesWhileAlive::PreInit(Scene* aScene, ParticleFactory* aFactory)
{
	myFactory = aFactory;
	myScene = aScene;
}


void ParticlesWhileAlive::Update(const float aDeltaTime)
{
	V3F epos = myEntity->GetPosition();
	Mesh* mesh = ((Mesh*)myEntity->GetComponent<Mesh>());
	
	V3F forward ;
	V3F right	;
	V3F up		;
	if (mesh)
	{
		CommonUtilities::Matrix4x4<float> mat = mesh->GetModelInstance()->GetModelToWorldTransform();
		forward = V4F(0, 0, 1, 0) * mat;
		right = V4F(1, 0, 0, 0) * mat;
		up = V4F(0, 1, 0, 0) * mat;
	}
	else
	{
		forward = myEntity->GetForward();
		right = myEntity->GetRight();
		up = myEntity->GetUp();
	}

	for (auto& i : myParticles)
	{
		if (!myScene->Contains(i.myParticle))
		{
			i.myParticle = nullptr;
		}
		if (i.myParticle)
		{
			V3F pos = epos + forward * i.myOffset.z + right * i.myOffset.x + up * i.myOffset.y;
			i.myParticle->SetBounds(V4F(pos + i.myBoundsMin, 1), V4F(pos + i.myBoundsMax, 1));
			i.myParticle->RefreshTimeout(10.5f);
		}
	}
}

void ParticlesWhileAlive::Reset()
{
	for (auto& i : myParticles)
	{
		if (myScene->Contains(i.myParticle))
		{
			i.myParticle->RefreshTimeout(-1.0f);
			i.myParticle = nullptr;
		}
	}
}

void ParticlesWhileAlive::SetParticle(std::string aParticle, V3F aBoundsMin, V3F aBoundsMax, V3F aOffset)
{
	handle newParticle;
	newParticle.myParticle = myFactory->InstantiateParticle(aParticle);

	newParticle.myParticle->RefreshTimeout(1000);

	if (newParticle.myParticle)
	{
		myScene->AddInstance(newParticle.myParticle);
	}
	newParticle.myBoundsMin = aBoundsMin;
	newParticle.myBoundsMax = aBoundsMin;
	newParticle.myOffset = aOffset;
	newParticle.myParticle->SetEternal(true);

	myParticles.push_back(newParticle);
}

void ParticlesWhileAlive::OnAttach()
{
}

void ParticlesWhileAlive::OnDetach()
{
}

void ParticlesWhileAlive::OnKillMe()
{
}