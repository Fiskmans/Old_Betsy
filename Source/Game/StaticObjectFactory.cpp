#include "pch.h"
#include "StaticObjectFactory.h"
#include "Entity.h"
#include "DataStructs.h"
#include "ObjectPool.hpp"
#include "ComponentLake.h"
#include "Mesh.h"
#include "Collision.h"
#include "GBPhysX.h"
#include "GBPhysXStaticComponent.h"
#include "House.h"
#include "GrowthSpot.h"
#include "FoodCellar.h"
#include "WaterSource.h"
#include "AnimationComponent.h"
#include "ChickenCoop.h"

StaticObjectFactory::StaticObjectFactory() :
	myOctree(nullptr),
	myEntityPool(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr)
{
}

StaticObjectFactory::~StaticObjectFactory()
{
	UnSubscribeToMessage(MessageType::SpawnStaticObject);
	UnSubscribeToMessage(MessageType::StartLoading);
	UnSubscribeToMessage(MessageType::UnloadLevel);
}

void StaticObjectFactory::Init(GBPhysX* aGBPhysX, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, UIManager* aUIManager)
{
	SubscribeToMessage(MessageType::SpawnStaticObject);
	SubscribeToMessage(MessageType::StartLoading);
	SubscribeToMessage(MessageType::UnloadLevel);

	myGBPhysX = aGBPhysX;
	myOctree = aOctTree;
	myEntityVector = aEntityVector;
	myEntityPool = aEntityPool;
	myEntityIDInt = aEntityIDInt;
	myUIManager = aUIManager;
}

void StaticObjectFactory::CreateStaticObject(std::string aFilePath, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale)
{
	PERFORMANCETAG("Object Creation");
	Entity* entity = myEntityPool->Retrieve();

	if (entity)
	{
		entity->Init(EntityType::EnvironmentStatic, *myEntityIDInt);
		*myEntityIDInt = *myEntityIDInt + 1;

		if (aFilePath == "G_soil_unplowed/G_Soil_Unplowed.fbx")
		{
			PERFORMANCETAG("Soil Edge case");
			entity->SetEntityType(EntityType::EnvironmentInteractable);
			GrowthSpot* spot = entity->AddComponent<GrowthSpot>();
			spot->Init(entity);
			spot->SetGBPhysXPointer(myGBPhysX);

			entity->AddComponent<Audio>()->Init(entity);
			entity->GetComponent<Audio>()->InternalInit(AudioComponentType::GrowSpot);

			entity->Spawn(aPos);
		}
		else
		{
			{
				PERFORMANCETAG("Component Attachment");
				V3F radRotation = TORAD(aRot);
				{
					PERFORMANCETAG("Mesh and spawn");
					entity->AddComponent<Mesh>()->Init(entity);
					entity->GetComponent<Mesh>()->SetUpModel(aFilePath);
					//entity->GetComponent<Mesh>()->SetPostRotation(radRotation);
					entity->GetComponent<Mesh>()->GetModelInstance()->Rotate(radRotation);
					entity->GetComponent<Mesh>()->SetScale(aScale);
					entity->SetScale(aScale);
					entity->Spawn(aPos);
					entity->SetRotation(radRotation);
					entity->SetSavedRotationValues(radRotation);
					entity->GetComponent<Mesh>()->Disable();
				}

				{
					PERFORMANCETAG("Physx comp");
					entity->AddComponent<GBPhysXStaticComponent>()->Init(entity);
					entity->GetComponent<GBPhysXStaticComponent>()->SetGBPhysXPtr(myGBPhysX);
					entity->GetComponent<GBPhysXStaticComponent>()->SetFilePath(aFilePath);
				}
			}

			{
				PERFORMANCETAG("Edge case finding");
				if (aFilePath == "P_cabin_01/P_Cabin_01.fbx")
				{
					entity->SetEntityType(EntityType::EnvironmentInteractable);
					entity->AddComponent<House>()->Init(entity);
					entity->GetComponent<House>()->SetScenePtr(myUIManager->GetScenePtr());
					entity->GetComponent<House>()->SetTextFactory(myUIManager->GetTextFactory());
					entity->myInteractPoint = { -141, 0, 770 };
				}
				if (aFilePath == "P_stash_01/P_Stash_01.fbx")
				{
					entity->SetEntityType(EntityType::EnvironmentInteractable);
					entity->AddComponent<FoodCellar>()->Init(entity);
					entity->GetComponent<FoodCellar>()->SetPtrs();
					myUIManager->SetEarthCellarEntity(entity);
					entity->myInteractPoint = { 707, 0, 2092 };

					entity->AddComponent<AnimationComponent>()->Init(entity);
					entity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
				}
				if (aFilePath == "P_waterstream_02/p_Waterstream_02.fbx")
				{
					entity->SetEntityType(EntityType::EnvironmentInteractable);
					entity->AddComponent<WaterSource>()->Init(entity);

					Message message;
					message.myMessageType = MessageType::RiverCreated;
					message.myData = entity;
					SendMessages(message);
				}
				if (aFilePath == "P_chickenhouse_01/P_chickenhouse_01.fbx")
				{
					entity->SetEntityType(EntityType::EnvironmentInteractable);
					entity->AddComponent<ChickenCoop>()->Init(entity);
					entity->myInteractPoint = { -969, 0, 293 };
				}
				if (aFilePath == "P_windturbine_01/P_WindTurbine_01.fbx")
				{
					entity->AddComponent<AnimationComponent>()->Init(entity);
					entity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
				}
			}
		}


		myEntityVector->push_back(entity);
		if (myCurrentLevel)
		{
			myCurrentLevel->push_back(entity);
		}
		else
		{
			SYSWARNING("Static object was loaded outside of levelstreaming, it wont be cleaned up properly", aFilePath);
		}
	}
	else
	{
		SYSWARNING("entity retrieved from entitypool was nullptr", aFilePath);
	}
}

void StaticObjectFactory::RecieveMessage(const Message& aMessage)
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
	case MessageType::SpawnStaticObject:
	{

		StaticObjectInstanceBuffer* buffer = static_cast<StaticObjectInstanceBuffer*>(aMessage.myData);
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> rot = { buffer->rotation[0],buffer->rotation[1], buffer->rotation[2] };
		CommonUtilities::Vector3<float> scale = { buffer->scale[0],buffer->scale[1], buffer->scale[2] };


		CreateStaticObject(buffer->aFilePath, pos, rot, scale);
	}
	break;
	}
}