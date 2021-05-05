#include "pch.h"
#include "DynamicObjectFactory.h"
#include "Entity.h"
#include "DataStructs.h"
#include "ObjectPool.hpp"
#include "ComponentLake.h"
#include "Mesh.h"
#include "Collision.h"
#include "GBPhysX.h"
#include "GBPhysXComponent.h"
#include "Random.h"

DynamicObjectFactory::DynamicObjectFactory() :
	myOctree(nullptr),
	myEntityPool(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr)
{
}

DynamicObjectFactory::~DynamicObjectFactory()
{
	UnSubscribeToMessage(MessageType::SpawnDynamicObject);
	UnSubscribeToMessage(MessageType::StartLoading);
	UnSubscribeToMessage(MessageType::UnloadLevel);
}

void DynamicObjectFactory::Init(GBPhysX* aGBPhysX, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt)
{
	SubscribeToMessage(MessageType::SpawnDynamicObject);
	SubscribeToMessage(MessageType::StartLoading);
	SubscribeToMessage(MessageType::UnloadLevel);

	myGBPhysX = aGBPhysX;
	myOctree = aOctTree;
	myEntityVector = aEntityVector;
	myEntityPool = aEntityPool;
	myEntityIDInt = aEntityIDInt;
}

void DynamicObjectFactory::CreateDynamicObject(std::string aFilePath, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale)
{
	const float degToRad = 57.2957f;
	Entity* entity = myEntityPool->Retrieve();

	if (entity)
	{
		entity->Init(EntityType::EnvironmentDynamic, *myEntityIDInt);
		*myEntityIDInt = *myEntityIDInt + 1;
		entity->AddComponent<Mesh>()->Init(entity);
		entity->GetComponent<Mesh>()->SetUpModel(aFilePath);
		entity->GetComponent<Mesh>()->GetModelInstance()->Rotate(CommonUtilities::Vector3<float>(aRot.x / degToRad, aRot.y / degToRad, aRot.z / degToRad));
		entity->GetComponent<Mesh>()->SetScale(aScale);
		entity->SetScale(aScale);
		entity->Spawn(aPos);
		entity->SetRotation(CommonUtilities::Vector3<float>(aRot.x / degToRad, aRot.y / degToRad, aRot.z / degToRad));
		entity->SetSavedRotationValues(CommonUtilities::Vector3<float>(aRot.x / degToRad, aRot.y / degToRad, aRot.z / degToRad));

		entity->AddComponent<GBPhysXComponent>()->Init(entity);

		V3F dir = myPlayerPtr->GetPosition() - aPos;
		dir.y = 0;
		dir.Normalize();
		dir *= 100.0f;

		float x = Tools::RandomRange(-120.0f, 120.0f);
		float z = Tools::RandomRange(-120.0f, 120.0f);
		entity->GetComponent<GBPhysXComponent>()->AttachGBPhysXActor(myGBPhysX->GBCreateDynamicBox(aPos + V3F(0.0f, 10.0f, 0.0f), V3F(10.0f, 10.0f, 10.0f), V3F(dir.x + x, 90.0f, dir.z + z), 2.0f));

		entity->AddComponent<LifeTime>()->Init(entity);
		entity->GetComponent<LifeTime>()->SetLifeTime(6.5f);

		myEntityVector->push_back(entity);
		if (myCurrentLevel)
		{
			myCurrentLevel->push_back(entity);
		}
		else
		{
			SYSWARNING("Dynamic object was loaded outside of levelstreaming, it wont be cleaned up properly", aFilePath);
		}
	}
	else
	{
		SYSWARNING("entity retrieved from entitypool was nullptr", aFilePath);
	}
}

void DynamicObjectFactory::SetPlayerPtr(Entity* aPlayerPtr)
{
	myPlayerPtr = aPlayerPtr;
}

void DynamicObjectFactory::RecieveMessage(const Message& aMessage)
{

	switch (aMessage.myMessageType)
	{
	case MessageType::StartLoading:
	{
		myCurrentLevel = new std::vector<Entity*>();
		myLevelObjectMapping[aMessage.myText.data()] = myCurrentLevel;
	}
	break;
	case MessageType::UnloadLevel:
	{
		if (myLevelObjectMapping.count(aMessage.myText.data()) != 0)
		{
			auto& toRemove = *myLevelObjectMapping[aMessage.myText.data()];
			for (auto& ent : toRemove)
			{
				for (int i = int(myEntityVector->size()) - 1; i >= 0; --i)
				{
					if (myEntityVector->operator[](i) == ent)
					{
						ent->Dispose();
						myEntityPool->Dispose(ent);
						myEntityVector->erase(myEntityVector->begin() + i);
					}
				}
			}
			delete myLevelObjectMapping[aMessage.myText.data()];
			myLevelObjectMapping.erase(aMessage.myText.data());
		}
		else
		{
			SYSWARNING("Unloading level thats not loaded", aMessage.myText.data());
		}
	}
	break;
	case MessageType::SpawnDynamicObject:
	{

		DynamicObjectInstanceBuffer* buffer = static_cast<DynamicObjectInstanceBuffer*>(aMessage.myData);
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> rot = { buffer->rotation[0],buffer->rotation[1], buffer->rotation[2] };
		CommonUtilities::Vector3<float> scale = { buffer->scale[0],buffer->scale[1], buffer->scale[2] };


		CreateDynamicObject(buffer->aFilePath, pos, rot, scale);
	}
	break;
	}
}