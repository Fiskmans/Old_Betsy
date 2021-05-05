#include "pch.h"
#include "Attached.h"
#include "Entity.h"
#include "Mesh.h"
#include <ModelInstance.h>

void Attached::SetupAttachement(Entity* aEntity, int aBoneIndex)
{
	myAttachedBoneIndex = aBoneIndex;
	myEntityAttachedTo = aEntity;
}

void Attached::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void Attached::Update(const float aDeltaTime)
{
	if (!myEntityAttachedTo)
	{
		return;
	}
	Mesh* mesh = myEntityAttachedTo->GetComponent<Mesh>();
	if (!mesh)
	{
		return;
	}
	ModelInstance* inst = mesh->GetModelInstance();
	if (!inst)
	{
		return;
	}

	M44F pos = inst->GetTransformOfBone(myAttachedBoneIndex);
}

void Attached::Reset()
{
	myAttachedBoneIndex = -1;
	myEntityAttachedTo = nullptr;
}

void Attached::OnAttach()
{
}

void Attached::OnDetach()
{
}

void Attached::OnKillMe()
{
}
