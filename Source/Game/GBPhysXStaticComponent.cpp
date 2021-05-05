#include "pch.h"
#include "GBPhysXStaticComponent.h"
#include "Mesh.h"
#include "GBPhysX.h"
#include <unordered_map>

void GBPhysXStaticComponent::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myHasSpawnedColliderToWorld = false;
}

void GBPhysXStaticComponent::Update(const float aDeltaTime)
{
	if (!myHasSpawnedColliderToWorld)
	{
		if (myFilePathSet)
		{
			if (myGBPhysX)
			{

				M44F mat;
				V3F rot = myEntity->GetSavedRotationValues();
				mat = M44F::CreateRotationAroundX(rot.x);
				mat *= M44F::CreateRotationAroundY(rot.y);
				mat *= M44F::CreateRotationAroundZ(rot.z);

				mat(4, 1) = myEntity->GetPosition().x;
				mat(4, 2) = myEntity->GetPosition().y;
				mat(4, 3) = myEntity->GetPosition().z;

				myGBPhysXActor = myGBPhysX->CreateStaticTriangleMeshObject(myFilePath, mat);
				if (myGBPhysXActor != nullptr)
				{
					myGBPhysXActor->SetEntity(myEntity);
					myHasSpawnedColliderToWorld = true;
				}
			}
		}
	}
	else
	{
		if (myPendingShapeNameChange)
		{
			myGBPhysXActor->SetShapeName(myPendingShapeNameChange);
			myPendingShapeNameChange = nullptr;
		}
	}
}

void GBPhysXStaticComponent::Reset()
{
	if (myGBPhysXActor)
	{
		myGBPhysXActor->RemoveFromScene();
		SAFE_DELETE(myGBPhysXActor);
	}

	myGBPhysX = nullptr;
	myMeshPtr = nullptr;
	myHasSpawnedColliderToWorld = false;
	myFilePath = "";
	myFilePathSet = false;
}

void GBPhysXStaticComponent::SetShapeName(const char* aName)
{
	myPendingShapeNameChange = aName;
}

void GBPhysXStaticComponent::SetFilePath(const std::string& aFilePath)
{
	myFilePath = aFilePath;
	myFilePathSet = true;
}

void GBPhysXStaticComponent::AttachGBPhysXActor(GBPhysXActor* aActor)
{
	if (myGBPhysXActor != nullptr)
	{
		myGBPhysXActor->RemoveFromScene();
		SAFE_DELETE(myGBPhysXActor);
		SYSERROR("ATTACH CALLED MORE THAN ONCE ON GB PHYSX COMPONENT, DANGER", "");
	}
	myGBPhysXActor = aActor;
	myMeshPtr = myEntity->GetComponent<Mesh>();
}

GBPhysXActor* GBPhysXStaticComponent::GetGBPhysXActor()
{
	return myGBPhysXActor;
}

void GBPhysXStaticComponent::SetGBPhysXPtr(GBPhysX* aGBPhysX)
{
	myGBPhysX = aGBPhysX;
}

void GBPhysXStaticComponent::OnAttach()
{

}

void GBPhysXStaticComponent::OnDetach()
{

}

void GBPhysXStaticComponent::OnKillMe()
{

}
