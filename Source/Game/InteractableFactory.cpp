#include "pch.h"
#include "InteractableFactory.h"
#include "Entity.h"
#include "DataStructs.h"
#include "CommonUtilities\ObjectPool.hpp"
#include "ComponentLake.h"
#include "Mesh.h"
#include "Collision.h"
#include "GBPhysX.h"
#include "GBPhysXStaticComponent.h"
#include "House.h"
#include "InteractImage.h"

InteractableFactory::InteractableFactory() :
	myOctree(nullptr),
	myEntityPool(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr)
{
}

InteractableFactory::~InteractableFactory()
{
	UnSubscribeToMessage(MessageType::SpawnInteractable);
	UnSubscribeToMessage(MessageType::StartLoading);
	UnSubscribeToMessage(MessageType::UnloadLevel);
}

void InteractableFactory::Init(GBPhysX* aGBPhysX, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt)
{
	SubscribeToMessage(MessageType::SpawnInteractable);
	SubscribeToMessage(MessageType::StartLoading);
	SubscribeToMessage(MessageType::UnloadLevel);

	myGBPhysX = aGBPhysX;
	myOctree = aOctTree;
	myEntityVector = aEntityVector;
	myEntityPool = aEntityPool;
	myEntityIDInt = aEntityIDInt;
}

void InteractableFactory::CreateInteractableObject(std::string aFilePath, int aInteractID, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, V3F aInteractPoint, std::string aImagePath)
{
	Entity* entity = myEntityPool->Retrieve();

	if (entity)
	{
		entity->Init(EntityType::EnvironmentInteractable, *myEntityIDInt);
		*myEntityIDInt = *myEntityIDInt + 1;

		entity->myInteractableID = aInteractID;

		entity->AddComponent<Mesh>()->Init(entity);
		entity->GetComponent<Mesh>()->SetUpModel(aFilePath);
		entity->GetComponent<Mesh>()->GetModelInstance()->Rotate(TORAD(aRot));
		entity->GetComponent<Mesh>()->SetScale(aScale);
		entity->GetComponent<Mesh>()->GetModelInstance()->SetIsHighlighted(true);
		entity->SetScale(aScale);
		entity->Spawn(aPos);
		entity->SetRotation(TORAD(aRot));
		entity->SetSavedRotationValues(TORAD(aRot));
		entity->GetComponent<Mesh>()->Disable();

		entity->AddComponent<GBPhysXStaticComponent>()->Init(entity);
		entity->GetComponent<GBPhysXStaticComponent>()->SetGBPhysXPtr(myGBPhysX);
		entity->GetComponent<GBPhysXStaticComponent>()->SetFilePath(aFilePath);
		entity->myInteractPoint = aInteractPoint;


		if (aImagePath != "none" && aImagePath != "None")
		{
			entity->AddComponent<InteractImage>()->Init(entity);
			entity->GetComponent<InteractImage>()->SetImagePath(aImagePath);
		}

		myEntityVector->push_back(entity);
		if (myCurrentLevel)
		{
			myCurrentLevel->push_back(entity);
		}
		else
		{
			SYSWARNING("Interactable object was loaded outside of levelstreaming, it wont be cleaned up properly", aFilePath);
		}
	}
	else
	{
		SYSWARNING("entity retrieved from entitypool was nullptr", aFilePath);
	}
}

void InteractableFactory::RecieveMessage(const Message& aMessage)
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
	case MessageType::SpawnInteractable:
	{

		InteractableInstanceBuffer* buffer = static_cast<InteractableInstanceBuffer*>(aMessage.myData);
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> rot = { buffer->rotation[0],buffer->rotation[1], buffer->rotation[2] };
		CommonUtilities::Vector3<float> scale = { buffer->scale[0],buffer->scale[1], buffer->scale[2] };
		int interactID = buffer->interactID;
		V3F interactPos = { buffer->interactPos[0], buffer->interactPos[1], buffer->interactPos[2] };
		std::string imagePath = buffer->aImagePath;

		CreateInteractableObject(buffer->aFilePath, interactID, pos, rot, scale, interactPos, imagePath);
	}
	break;
	}
}