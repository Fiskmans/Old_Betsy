#include "pch.h"
#include "TriggerBoxFactory.h"
#include "Entity.h"
#include "DataStructs.h"
#include "CommonUtilities\ObjectPool.hpp"
#include "ComponentLake.h"
#include "Collision.h"

std::vector<Entity*> TriggerBoxFactory::myDebugVector;

TriggerBoxFactory::TriggerBoxFactory() :
	myOctree(nullptr),
	myEntityPool(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr),
	mySplines(nullptr)
{
}

TriggerBoxFactory::~TriggerBoxFactory()
{
	UnSubscribeToMessage(MessageType::SpawnTriggerBox);
	UnSubscribeToMessage(MessageType::NewLevelLoaded);
}

void TriggerBoxFactory::Init(Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt)
{
	SubscribeToMessage(MessageType::SpawnTriggerBox);
	SubscribeToMessage(MessageType::NewLevelLoaded);

	myOctree = aOctTree;
	myEntityVector = aEntityVector;
	myEntityPool = aEntityPool;
	myEntityIDInt = aEntityIDInt;
}

void TriggerBoxFactory::CreateTriggerBox(std::string aFilePath, int aTriggerBoxID, int aEventID, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, bool aIsStrikeable)
{
	Entity* entity = myEntityPool->Retrieve();
	entity->Init(EntityType::TriggerBox, *myEntityIDInt);
	*myEntityIDInt = *myEntityIDInt + 1;
	entity->SetScale(aScale);
	entity->SetRotation(CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f));
	entity->Spawn(aPos);
	
	CommonUtilities::Vector3<float> aBBMin;
	aBBMin.x = aPos.x - (aScale.x / 2);
	aBBMin.y = aPos.y - (aScale.y / 2);
	aBBMin.z = aPos.z - (aScale.z / 2);
	
	CommonUtilities::Vector3<float> aBBMax;
	aBBMax.x = aPos.x + (aScale.x / 2);
	aBBMax.y = aPos.y + (aScale.y / 2);
	aBBMax.z = aPos.z + (aScale.z / 2);
	
	entity->AddComponent<Collision>()->Init(entity);
	entity->GetComponent<Collision>()->SetStaticBoundingBox(aBBMin, aBBMax);
	entity->GetComponent<Collision>()->SetIsFriendly(false);
	
	entity->GetComponent<Collision>()->SetTriggerID(aEventID);
	entity->GetComponent<Collision>()->SetIsStrikeableTrigger(aIsStrikeable);
	myEntityVector->push_back(entity);
	myDebugVector.push_back(entity);
}

void TriggerBoxFactory::DrawTriggerBoxes()
{
	for (auto& i : myDebugVector)
	{
		DebugDrawer::GetInstance().DrawBoundingBox(*i->GetComponent<Collision>()->GetBoxCollider());
	}
}

void TriggerBoxFactory::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::SpawnTriggerBox)
	{
		TriggerBoxInstanceBuffer* buffer = static_cast<TriggerBoxInstanceBuffer*>(aMessage.myData);
		float ID = buffer->ID;
		float targetID = buffer->targetID;
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> rot = { buffer->rotation[0],buffer->rotation[1], buffer->rotation[2] };
		CommonUtilities::Vector3<float> scale = { buffer->scale[0],buffer->scale[1], buffer->scale[2] };
		bool isStrikeable = buffer->isAbilityTriggered;

		CreateTriggerBox(buffer->aFilePath, CAST(int, ID), CAST(int, targetID), pos, rot, scale, isStrikeable);
	}
	if (aMessage.myMessageType == MessageType::NewLevelLoaded)
	{
		myDebugVector.clear();
	}
}