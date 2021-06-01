#include "pch.h"
#include "GBPhysXComponent.h"
#include "Mesh.h"
#include "GBPhysX.h"

void GBPhysXComponent::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void GBPhysXComponent::Update(const float aDeltaTime)
{
	myEntity->SetRotation(myGBPhysXActor->GetTransformMatrix());
	myEntity->SetPosition(myGBPhysXActor->GetPosition());
}

void GBPhysXComponent::Reset()
{
	myGBPhysXActor->RemoveFromScene();
	SAFE_DELETE(myGBPhysXActor);
	myMeshPtr = nullptr;
}

void GBPhysXComponent::AttachGBPhysXActor(GBPhysXActor* aActor)
{
	if (myGBPhysXActor != nullptr)
	{
		myGBPhysXActor->RemoveFromScene();
		SAFE_DELETE(myGBPhysXActor);
		SYSERROR("ATTACH CALLED MORE THAN ONCE ON GB PHYSX COMPONENT");
	}
	myGBPhysXActor = aActor;
	myMeshPtr = myEntity->GetComponent<Mesh>();
}

GBPhysXActor* GBPhysXComponent::GetGBPhysXActor()
{
	return myGBPhysXActor;
}

void GBPhysXComponent::AddForceToActor(V3F aDirection, float aMagnitude)
{
	if (myGBPhysXActor)
	{
		myGBPhysXActor->ApplyForce(aDirection, aMagnitude);
	}
}

void GBPhysXComponent::OnAttach()
{

}

void GBPhysXComponent::OnDetach()
{

}

void GBPhysXComponent::OnKillMe()
{

}
