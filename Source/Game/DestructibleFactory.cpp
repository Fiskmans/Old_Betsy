#include "pch.h"
#include "DestructibleFactory.h"
#include "Entity.h"
#include "DataStructs.h"
#include "CommonUtilities\ObjectPool.hpp"
#include "ComponentLake.h"
#include "Mesh.h"
#include "Collision.h"
#include "CharacterData.h"
#include "Random.h"
#include "Octree.h"
#include "Movement3D.h"
#include "Audio.h"
#include "Life.h"
#include "CharacterInstance.h"

DestructibleFactory::DestructibleFactory() :
	myOctree(nullptr),
	myEntityPool(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr),
	myCharacterData(nullptr)
{
}

DestructibleFactory::~DestructibleFactory()
{
	UnSubscribeToMessage(MessageType::SpawnDestructibleObject);
	UnSubscribeToMessage(MessageType::NewOctreeCreated);
}

void DestructibleFactory::Init(Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, CharacterData* aCharacterData)
{
	SubscribeToMessage(MessageType::SpawnDestructibleObject);
	SubscribeToMessage(MessageType::NewOctreeCreated);

	myOctree = aOctTree;
	myEntityVector = aEntityVector;
	myEntityPool = aEntityPool;
	myEntityIDInt = aEntityIDInt;
	myCharacterData = aCharacterData;
}

void DestructibleFactory::CreateDestructibleObject(std::string aFilePath, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, int aType)
{
	Entity* entity = myEntityPool->Retrieve();

	if (entity)
	{
		entity->Init(EntityType::EnvironmentDestructible, *myEntityIDInt);
		*myEntityIDInt = *myEntityIDInt + 1;
		entity->SetDestructibleType(aType);
		
		entity->SetScale(aScale);
		/*if (aType == 1)
		{
			entity->AddComponent<CharacterInstance>()->Init(entity, myCharacterData->GetCharacterStats(99), myAbilityData, 1);
		}
		else if (aType == 2)
		{
			entity->AddComponent<CharacterInstance>()->Init(entity, myCharacterData->GetCharacterStats(101), myAbilityData, 1);
		}
		else
		{
			entity->AddComponent<CharacterInstance>()->Init(entity, myCharacterData->GetCharacterStats(100), myAbilityData, 1);
		}*/
		entity->AddComponent<Mesh>()->Init(entity);
		entity->GetComponent<Mesh>()->SetUpModel(entity->GetComponent<CharacterInstance>()->GetCharacterStats()->fbxPath);
		entity->GetComponent<Mesh>()->GetModelInstance()->Rotate(TORAD(aRot) + V3F(0,PI,0));
		entity->GetComponent<Mesh>()->SetScale(aScale);

		entity->AddComponent<Life>()->Init(entity, entity->GetComponent<CharacterInstance>()->GetCharacterStats()->life);
		entity->AddComponent<Movement3D>()->Init(entity);
		entity->Spawn(aPos);
		entity->SetRotation(TORAD(aRot) + V3F(0, PI, 0));
		entity->SetSavedRotationValues(TORAD(aRot) + V3F(0, PI, 0));
		entity->AddComponent<Collision>()->Init(entity);
		entity->GetComponent<Collision>()->SetCollisionRadius(70.0f);
		entity->GetComponent<Collision>()->SetHeightOffset(30.0f);
		entity->GetComponent<Collision>()->SetIsFriendly(false);

		entity->AddComponent<Audio>()->Init(entity);
		entity->GetComponent<Audio>()->InternalInit(AudioComponentType::None);

		entity->SetIsMoving(true);
		//entity->GetComponent<Mesh>()->Disable();
		myEntityVector->push_back(entity);

		if (myOctree != nullptr)
		{
			if (myOctree->ourTreeBuilt)
			{
				myOctree->ourPendingInsertionList.push(entity->GetComponent<Collision>());
			}
		}
	}
	else
	{
		SYSWARNING("entity retrieved from entitypool was nullptr","");
	}

}

void DestructibleFactory::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::SpawnDestructibleObject)
	{
		DestructibleObjectInstanceBuffer* buffer = static_cast<DestructibleObjectInstanceBuffer*>(aMessage.myData);
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> rot = { buffer->rotation[0],buffer->rotation[1], buffer->rotation[2] };
		CommonUtilities::Vector3<float> scale = { buffer->scale[0],buffer->scale[1], buffer->scale[2] };
		int destructibleType = buffer->type;

		CreateDestructibleObject(buffer->aFilePath, pos, rot, scale, destructibleType);
	}
	else if (aMessage.myMessageType == MessageType::NewOctreeCreated)
	{
		myOctree = (Octree*)aMessage.myData;
	}
}
