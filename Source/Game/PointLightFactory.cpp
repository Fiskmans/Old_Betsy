#include "pch.h"
#include "PointLightFactory.h"
#include "Entity.h"
#include "DataStructs.h"
#include "ObjectPool.hpp"
#include "Light.h"


PointLightFactory::PointLightFactory() :
	myComponentLake(nullptr),
	myEntityPoolPtr(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr)
{
}

PointLightFactory::~PointLightFactory()
{
	UnSubscribeToMessage(MessageType::SpawnPointLight);
}

void PointLightFactory::Init(std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt)
{
	SubscribeToMessage(MessageType::SpawnPointLight);

	myEntityVector = aEntityVector;
	myEntityPoolPtr = aEntityPool;
	myComponentLake = aComponentLake;
	myEntityIDInt = aEntityIDInt;
}

void PointLightFactory::CreatePointLight(CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aColor, float aIntensity, float aPeriod, float aMinPercent , float aRange, int id)
{
	Entity* entity = myEntityPoolPtr->Retrieve();
	entity->Init(EntityType::None, *myEntityIDInt);
	*myEntityIDInt = *myEntityIDInt + 1;
	entity->AddComponent<Light>()->Init(entity);
	Light* lightComponent = entity->GetComponent<Light>();
	lightComponent->SetColor(aColor);
	lightComponent->SetIntensity(aIntensity);
	lightComponent->SetPeriod(aPeriod);
	lightComponent->SetRange(aRange);
	lightComponent->SetMinIntensity(aMinPercent);
	lightComponent->SetID(id);

	entity->Spawn(aPos);
	myEntityVector->push_back(entity);
}

void PointLightFactory::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::SpawnPointLight)
	{
		PointLightInstanceBuffer* buffer = static_cast<PointLightInstanceBuffer*>(aMessage.myData);
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> color = { buffer->color[0],buffer->color[1], buffer->color[2] };
		float intensity = { buffer->intensity * 1000 };
		float period = { buffer->period };
		float minPercent = { buffer->minVal };
		float range = { buffer->range * 100};
		int id = { buffer->id };
		CreatePointLight(pos, color, intensity, period, minPercent , range, id);
	}
}
