#include "pch.h"
#include "Entity.h"
#include "Component.h"
#include "Macros.h"
#include "Random.h"
#include "Mesh.h"
#include "Movement3D.h"
#include "Growable.h"
#include "GrowthSpot.h"
#include "House.h"

Entity::Entity() :
	myIsAlive(false),
	myActiveThings(0),
	mySpawnTime(0),
	myEntityType(EntityType::None),
	myEntityID(0),
	myDestructibleType(0),
	mySavedRotationValues(V3F())
{
	mySpawnPos = CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f);
	myPosition = mySpawnPos;
}

Entity::~Entity()
{
	RemoveAllComponents();
}

void Entity::Init(EntityType aEntityType, unsigned int aEntityID, unsigned int aNodeID)
{
	myEntityType = aEntityType;
	myEntityID = aEntityID;
	myNodeID = aNodeID;
	myIsAlive = true;
}

void Entity::SetEntityType(EntityType aEntityType)
{
	myEntityType = aEntityType;
}

void Entity::Spawn()
{
	myPosition = mySpawnPos;
	myIsAlive = true;
	SetSpawnTime(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count());
}

void Entity::Spawn(const CommonUtilities::Vector3<float>& aPosition)
{
	PERFORMANCETAG("Spawn");
	myPosition = aPosition;
	SetSpawnPos(aPosition);
	myIsAlive = true;

	{

		Growable* growable;
		GrowthSpot* growthSpot;
		Mesh* meshComponent;

		PERFORMANCETAG("component setup");
		{
			PERFORMANCETAG("Fetching");
			growable = GetComponent<Growable>();
			growthSpot = GetComponent<GrowthSpot>();
			meshComponent = GetComponent<Mesh>();
		}
		{
			PERFORMANCETAG("CustomActions");
			if (growable)
			{
				growable->Spawn();
			}
			if (growthSpot)
			{
				growthSpot->Spawn();
			}
			if (meshComponent != nullptr)
			{
				PERFORMANCETAG("Mesh action");
				meshComponent->GetModelInstance()->SetPosition({ myPosition.x, myPosition.y, myPosition.z, 1 });
				meshComponent->AddModelToScene();
			}
		}
	}
	/*auto audioComponent = GetComponent<Audio>();
	if (audioComponent)
	{
		audioComponent->PlayAudioEvent(AudioEvent::Spawn);
	}*/
	{
		PERFORMANCETAG("Spawn time");
		SetSpawnTime(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count());
	}
}

void Entity::SendEntityMessage(EntityMessage aMessage, void* someData)
{
	for (auto& i : myComponents)
	{
		i.second->RecieveEntityMessage(aMessage, someData);
	}
}

void Entity::RemoveAllComponents()
{
	for (auto& [componentId, component] : myComponents)
	{
		component->Reset();
		ComponentLake::GetInstance().ReturnComponent(componentId, component);
	}
	myComponents.clear();
}

void Entity::DisableAll()
{
	for (auto& component : myComponents)
	{
		component.second->Disable();
	}
}

void Entity::EnableAll()
{
	for (auto& component : myComponents)
	{
		component.second->Enable();
	}
}

void Entity::SetSpawnPos(CommonUtilities::Vector3<float> aSpawnPos)
{
	mySpawnPos = aSpawnPos;
}

void Entity::Dispose()
{
	RemoveAllComponents();
	myEntityID = 0;
	myEntityType = EntityType::None;
	//myEnemyType = EnemyType::None;
	myIsAlive = false;
	myIsMoving = false;
	myPosition = mySpawnPos;
	myActiveThings = 0;
}



V3F Entity::GetForward()
{
	CommonUtilities::Vector4<float> forward(0, 0, 1, 0);
	forward = forward * myRotationMatrix;
	return CommonUtilities::Vector3<float>(forward.x, forward.y, forward.z);
}
V3F Entity::GetBack()
{
	CommonUtilities::Vector4<float> forward(0, 0, -1, 0);
	forward = forward * myRotationMatrix;
	return CommonUtilities::Vector3<float>(forward.x, forward.y, forward.z);
}
V3F Entity::GetUp()
{
	CommonUtilities::Vector4<float> forward(0, 1, 0, 0);
	forward = forward * myRotationMatrix;
	return CommonUtilities::Vector3<float>(forward.x, forward.y, forward.z);
}
V3F Entity::GetDown()
{
	CommonUtilities::Vector4<float> forward(0, -1, 0, 0);
	forward = forward * myRotationMatrix;
	return CommonUtilities::Vector3<float>(forward.x, forward.y, forward.z);
}
V3F Entity::GetLeft()
{
	CommonUtilities::Vector4<float> forward(-1, 0, 0, 0);
	forward = forward * myRotationMatrix;
	return CommonUtilities::Vector3<float>(forward.x, forward.y, forward.z);
}
V3F Entity::GetRight()
{
	CommonUtilities::Vector4<float> forward(1, 0, 1, 0);
	forward = forward * myRotationMatrix;
	return CommonUtilities::Vector3<float>(forward.x, forward.y, forward.z);
}

bool Entity::GetIsAlive()
{
	return myIsAlive;
}

bool Entity::GetShouldBeRemoved()
{
	if (!GetIsAlive())
	{
		int i = 0;
		if (myEntityType == EntityType::EnvironmentInteractable)
		{
			++i;
		}
	}
	return !GetIsAlive() && myActiveThings == 0;
}

void Entity::SetIsAlive(const bool aIsAlive)
{
	myIsAlive = aIsAlive;
}

void Entity::Kill()
{
	SetIsAlive(false);
	SetRotation(V3F(0, 0, 0));

	/*auto audioComponent = GetComponent<Audio>();
	if (audioComponent)
	{
		audioComponent->PlayAudioEvent(AudioEvent::Death);
	}*/
}

bool Entity::GetIsMoving()
{
	return myIsMoving;
}

void Entity::SetIsMoving(const bool aIsMoving)
{
	myIsMoving = aIsMoving;
}

CommonUtilities::Vector3<float> Entity::GetPosition()
{
	return myPosition;
}

void Entity::SetPosition(CommonUtilities::Vector3<float> aPosition)
{
	myPosition = aPosition;
}

CommonUtilities::Matrix4x4<float> Entity::GetRotation()
{
	return myRotationMatrix;
}

void Entity::SetRotation(CommonUtilities::Vector3<float> aRotation)
{
	CommonUtilities::Matrix4x4<float> mat;
	mat.RotateAroundX(aRotation.x);
	mat.RotateAroundY(aRotation.y);
	mat.RotateAroundZ(aRotation.z);
	
	myRotationMatrix = mat;
}

void Entity::SetRotation(CommonUtilities::Matrix4x4<float> aRotationMatrix)
{
	myRotationMatrix = aRotationMatrix;
}

void Entity::SetSavedRotationValues(CommonUtilities::Vector3<float> aRot)
{
	mySavedRotationValues = aRot;
}

CommonUtilities::Vector3<float> Entity::GetSavedRotationValues()
{
	return mySavedRotationValues;
}

CommonUtilities::Vector3<float> Entity::GetScale()
{
	return myScale;
}

void Entity::SetScale(CommonUtilities::Vector3<float> aScale)
{
	Mesh* mesh = GetComponent<Mesh>();
	if (mesh)
	{
		mesh->GetModelInstance()->SetScale(aScale);
	}
	myScale = aScale;
}

EntityType Entity::GetEntityType()
{
	return myEntityType;
}

int Entity::GetEntityID()
{
	return myEntityID;
}

void Entity::SetDestructibleType(const int aType)
{
	myDestructibleType = aType;
}

int Entity::GetDestructibleType()
{
	return myDestructibleType;
}

void Entity::SetTargetPosition(V3F aPosition)
{
	Movement3D* move = GetComponent<Movement3D>();
	if (move)
	{
		move->SetTargetPosition(aPosition);
	}
}

unsigned int Entity::GetNodeID() const
{
	return myNodeID;
}

#if USEIMGUI
void Entity::ImGuiNode(ImGuiNodePackage& aPackage)
{
	ImGui::PushID(myEntityID);
	if (ImGui::TreeNode("", "id: %d", myEntityID))
	{

		for (auto& i : myComponents)
		{
			i.second->ImGuiNode(aPackage);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}
#endif // !USEIMGUI
void Entity::AddActivity()
{
	myActiveThings++;
}

void Entity::FinishActivity()
{
	if (myActiveThings > 0)
	{
		myActiveThings--;
	}
	else
	{
		SYSWARNING("Entity would have landed on -1 thing left to do","");
	}
}
void Entity::SetSpawnTime(long long aTime)
{
	mySpawnTime = aTime;
}
long long Entity::GetSpawnTime()
{
	return mySpawnTime;
}

void Entity::InteractWith(Item* aHeldTool)
{
	for (auto& i : myComponents)
	{
		i.second->InteractWith(aHeldTool);
	}
}

void Entity::MoveToSpawn()
{
	myPosition = mySpawnPos;
}
