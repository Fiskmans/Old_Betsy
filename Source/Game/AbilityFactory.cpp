#include "pch.h"
#include "AbilityFactory.h"
#include "Entity.h"
#include "ObjectPool.hpp"
#include "ComponentLake.h"
#include "Mesh.h"
#include "LifeTime.h"
#include "Collision.h"
#include "AbilityData.h"
#include "CharacterInstance.h"
#include "AbilityInstance.h"
#include "PlayerController.h"
#include "AnimationComponent.h"
#include "Audio.h"
#include "Model.h"
#include "Light.h"
#include "ParticlesWhileAlive.h"

AbilityFactory::AbilityFactory() :
	myEntityPoolPtr(nullptr),
	myEntityIDInt(nullptr),
	myEntityVector(nullptr),
	myComponentLake(nullptr),
	myAbilityData(nullptr),
	myPlayerPtr(nullptr),
	myAbilityVFXVector(nullptr)
{
}

AbilityFactory::~AbilityFactory()
{
	UnSubscribeToMessage(MessageType::SpawnAbility);
	UnSubscribeToMessage(MessageType::SpawnAbilityAt);
}

void AbilityFactory::Init(std::vector<Entity*>* aEntityVector, std::vector<Entity*>* aAbilityVFXVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, AbilityData* aAbilityData)
{
	myEntityVector = aEntityVector;
	myAbilityVFXVector = aAbilityVFXVector;
	myEntityPoolPtr = aEntityPool;
	myComponentLake = aComponentLake;
	myEntityIDInt = aEntityIDInt;
	myAbilityData = aAbilityData;
	SubscribeToMessage(MessageType::SpawnAbility);
	SubscribeToMessage(MessageType::SpawnAbilityAt);
}

void AbilityFactory::CreateAbility(Entity* aCastingEntity, V3F aDirection, int aAbilityType)
{

	Entity* entity = myEntityPoolPtr->Retrieve();
	entity->Init(EntityType::Ability, (*myEntityIDInt)++);
	entity->SetRotation(aCastingEntity->GetRotation());

	entity->AddComponent<AbilityInstance>()->Init(entity, &myAbilityData->GetAbility(aAbilityType), aCastingEntity, (aCastingEntity->GetComponent<CharacterInstance>()->GetCharacterStats()));

	Ability* ability = &myAbilityData->GetAbility(entity->GetComponent<AbilityInstance>()->GetAbilityID());

	entity->AddComponent<Mesh>()->Init(entity);
	entity->GetComponent<Mesh>()->SetUpModel(ability->fbxPath);
	entity->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
	entity->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);
	
	if (ability->isAnimated)
	{
		entity->AddComponent<AnimationComponent>()->Init(entity);
	}

	entity->AddComponent<LifeTime>()->Init(entity);
	entity->GetComponent<LifeTime>()->SetLifeTime(ability->duration);

	entity->AddComponent<Collision>()->Init(entity);

	entity->AddComponent<Audio>()->Init(entity);

	if (aCastingEntity->GetEntityType() == EntityType::Player)
	{
		entity->GetComponent<Collision>()->SetIsFriendly(true);
	}
	else
	{
		entity->GetComponent<Collision>()->SetIsFriendly(false);
	}
	entity->GetComponent<Collision>()->SetDamageToOthers(CAST(int, myAbilityData->GetAbility(aAbilityType).damageMin));

	if (aAbilityType == 5)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->width * 0.5f);
		entity->GetComponent<Collision>()->SetHeightOffset(50.0f);
		entity->Spawn(aCastingEntity->GetPosition() + aDirection * std::sqrt(ability->rangeSqrd));
	}
	else if (aAbilityType == 6)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->width * 0.5f);
		entity->GetComponent<Collision>()->SetHeightOffset(25.0f);
		entity->Spawn(aCastingEntity->GetPosition() + aDirection * std::sqrt(ability->rangeSqrd));
	}
	else if (aAbilityType == 1)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->radius);
		entity->Spawn(aCastingEntity->GetPosition());

		// EXTRA ABILITY VFX -------------------------------------------------------

		entity->AddComponent<Light>()->Init(entity);
		Light* abilityLight = entity->GetComponent<Light>();
		abilityLight->SetColor(V3F(1.0f, 0.14f, 0.0f));
		abilityLight->SetRange(400);
		abilityLight->SetIntensity(6000);
		abilityLight->SetOffset(V3F(0.0f, 170.0f, 0.0f));

		Entity* entityVFX = myEntityPoolPtr->Retrieve();
		entityVFX->Init(EntityType::Ability, (*myEntityIDInt)++);
		//entityVFX->SetRotation(aCastingEntity->GetRotation());
		entityVFX->SetRotation(V3F());

		entityVFX->AddComponent<Mesh>()->Init(entityVFX);
		entityVFX->GetComponent<Mesh>()->SetUpModel(ability->animationPath);
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);
		if (ability->isAnimated)
		{
			entityVFX->AddComponent<AnimationComponent>()->Init(entityVFX);
		}

		entityVFX->AddComponent<LifeTime>()->Init(entityVFX);
		entityVFX->GetComponent<LifeTime>()->SetLifeTime(ability->duration * 1.0f);

		entityVFX->Spawn(aCastingEntity->GetPosition());
		myAbilityVFXVector->push_back(entityVFX);
	}
	else if (aAbilityType == 2)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->radius);
		entity->Spawn(aCastingEntity->GetPosition());

		// EXTRA ABILITY VFX -------------------------------------------------------

		Entity* entityVFX = myEntityPoolPtr->Retrieve();
		entityVFX->Init( EntityType::Ability, (*myEntityIDInt)++);
		//entityVFX->SetRotation(aCastingEntity->GetRotation());
		entityVFX->SetRotation(V3F());

		entityVFX->AddComponent<Mesh>()->Init(entityVFX);
		entityVFX->GetComponent<Mesh>()->SetUpModel(ability->animationPath);
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);
		if (ability->isAnimated)
		{
			entityVFX->AddComponent<AnimationComponent>()->Init(entityVFX);
		}

		entityVFX->AddComponent<LifeTime>()->Init(entityVFX);
		entityVFX->GetComponent<LifeTime>()->SetLifeTime(ability->duration * 1.0f);

		entityVFX->Spawn(aCastingEntity->GetPosition());
		myAbilityVFXVector->push_back(entityVFX);

		entity->AddComponent<Light>()->Init(entity);
		Light* abilityLight = entity->GetComponent<Light>();
		abilityLight->SetColor(V3F(1.0f, 0.0f, 0.0f));
		abilityLight->SetRange(700);
		abilityLight->SetIntensity(12000);
		abilityLight->SetOffset(V3F(0.0f, 200.0f, 0.0f));
	}
	else if (aAbilityType == 3)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->radius);
		V3F targetPos = { 0,0,0 };//aCastingEntity->GetComponent<PlayerController>()->GetCurrentTargetedPos();
		V3F distance = targetPos - aCastingEntity->GetPosition();

		float lengthSqr = distance.LengthSqr();
		if (lengthSqr > ability->rangeSqrd)
		{
			V3F dir = distance.GetNormalized();
			targetPos = aCastingEntity->GetPosition() + (dir * (sqrt(ability->rangeSqrd)));
		}

		V3F	pos = PathFinder::GetInstance().Floorify(targetPos);
		if (pos != V3F())
		{
			entity->Spawn(pos);
		}
		else
		{
			entity->Spawn(targetPos);
		}

		// EXTRA ABILITY VFX -------------------------------------------------------

		Entity* entityVFX = myEntityPoolPtr->Retrieve();
		entityVFX->Init(EntityType::Ability, (*myEntityIDInt)++);
		//entityVFX->SetRotation(aCastingEntity->GetRotation());
		entityVFX->SetRotation(V3F());

		entityVFX->AddComponent<Mesh>()->Init(entityVFX);
		entityVFX->GetComponent<Mesh>()->SetUpModel(ability->animationPath);
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);
		if (ability->isAnimated)
		{
			entityVFX->AddComponent<AnimationComponent>()->Init(entityVFX);
		}

		entityVFX->AddComponent<LifeTime>()->Init(entityVFX);
		entityVFX->GetComponent<LifeTime>()->SetLifeTime(ability->duration * 1.0f);

		if (pos != V3F())
		{
			entityVFX->Spawn(pos);
		}
		else
		{
			entityVFX->Spawn(targetPos);
		}

		myAbilityVFXVector->push_back(entityVFX);
	}

	else if (aAbilityType == 4)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(150.0f);
		entity->Spawn(aCastingEntity->GetPosition() + aDirection * 150.0f);
	}

	else if (aAbilityType == 7)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(50.0f);
		entity->GetComponent<Collision>()->SetHeightOffset(50.0f);
		entity->Spawn(aCastingEntity->GetPosition() + aDirection * 100.0f);
	}

	else if (aAbilityType == 10)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->width * 0.5f);
		entity->GetComponent<Collision>()->SetHeightOffset(50.0f);
		entity->Spawn(aCastingEntity->GetPosition() + aDirection * std::sqrt(ability->rangeSqrd));
	}

	else if (aAbilityType == 81)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->radius);
		V3F targetPos = myPlayerPtr->GetPosition();
		V3F distance = targetPos - aCastingEntity->GetPosition();

		float lengthSqr = distance.LengthSqr();
		if (lengthSqr > ability->rangeSqrd)
		{
			V3F dir = distance.GetNormalized();
			targetPos = aCastingEntity->GetPosition() + (dir * (sqrt(ability->rangeSqrd)));
		}

		V3F pos = myPlayerPtr->GetPosition();
		V3F testPos = PathFinder::GetInstance().Floorify(pos);
		if (testPos != V3F())
		{
			entity->Spawn(testPos);
		}
		else
		{
			entity->Spawn(myPlayerPtr->GetPosition());
		}

		// EXTRA ABILITY VFX -------------------------------------------------------

		Entity* entityVFX = myEntityPoolPtr->Retrieve();
		entityVFX->Init(EntityType::Ability, (*myEntityIDInt)++);
		entityVFX->SetRotation(V3F());

		entityVFX->AddComponent<Mesh>()->Init(entityVFX);
		entityVFX->GetComponent<Mesh>()->SetUpModel(ability->animationPath);
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);
		if (ability->isAnimated)
		{
			entityVFX->AddComponent<AnimationComponent>()->Init(entityVFX);
		}

		entityVFX->AddComponent<LifeTime>()->Init(entityVFX);
		entityVFX->GetComponent<LifeTime>()->SetLifeTime(ability->duration * 1.0f);

		if (testPos != V3F())
		{
			entityVFX->Spawn(testPos);
		}
		else
		{
			entityVFX->Spawn(myPlayerPtr->GetPosition());
		}
		myAbilityVFXVector->push_back(entityVFX);
	}
	else if (aAbilityType == 11)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->radius);

		V3F pos = myPlayerPtr->GetPosition();
		V3F testPos = PathFinder::GetInstance().Floorify(pos);
		if (testPos != V3F())
		{
			entity->Spawn(testPos);
		}
		else
		{
			entity->Spawn(myPlayerPtr->GetPosition());
		}
	}

	else if (aAbilityType > 10)
	{
		entity->GetComponent<Collision>()->SetCollisionRadius(ability->width * 0.5f);
		entity->GetComponent<Collision>()->SetHeightOffset(50.0f);
		entity->Spawn(aCastingEntity->GetPosition() + aDirection * std::sqrt(ability->rangeSqrd));
	}

	//entity->SetRotation(aCastingEntity->GetForward());

	myEntityVector->push_back(entity);
}

Entity* AbilityFactory::CreateAbility(V3F aPosition, V3F aSize, int aAbilityType)
{
	Entity* entity = myEntityPoolPtr->Retrieve();
	entity->Init(EntityType::Ability, (*myEntityIDInt)++);

	entity->AddComponent<AbilityInstance>()->Init(entity, &myAbilityData->GetAbility(aAbilityType), myPlayerPtr, myPlayerPtr->GetComponent<CharacterInstance>()->GetCharacterStats());

	Ability* ability = &myAbilityData->GetAbility(entity->GetComponent<AbilityInstance>()->GetAbilityID());

	entity->AddComponent<Mesh>()->Init(entity);
	entity->GetComponent<Mesh>()->SetUpModel(ability->fbxPath);
	entity->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
	entity->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);

	entity->AddComponent<ParticlesWhileAlive>()->Init(entity);
	entity->GetComponent<ParticlesWhileAlive>()->SetParticle("FireTrap.part");

	entity->AddComponent<LifeTime>()->Init(entity);
	entity->GetComponent<LifeTime>()->SetLifeTime(ability->duration * 1.0f);

	entity->AddComponent<Collision>()->Init(entity);

	entity->AddComponent<Audio>()->Init(entity);
	//entity->GetComponent<Audio>()->InitEventIDs();

	//TODO FIX DAMAGE TO ALL
	entity->GetComponent<Collision>()->SetIsFriendly(true);

	entity->GetComponent<Collision>()->SetDamageToOthers(CAST(int, myAbilityData->GetAbility(aAbilityType).damageMin));

	//COLLISION SIZE AND SPAWNING
	entity->GetComponent<Collision>()->SetCollisionRadius(ability->width * 0.5f);
	entity->GetComponent<Collision>()->SetHeightOffset(50.0f);
	entity->SetRotation(V3F());
	entity->Spawn(aPosition);
	
	myEntityVector->push_back(entity);
	return entity;
}

void AbilityFactory::CreateAbility(Entity* aCastingEntity, V3F aPosition, int aAbilityType, bool aVal)
{
	Entity* entity = myEntityPoolPtr->Retrieve();
	entity->Init(EntityType::Ability, (*myEntityIDInt)++);

	entity->AddComponent<AbilityInstance>()->Init(entity, &myAbilityData->GetAbility(aAbilityType), aCastingEntity, aCastingEntity->GetComponent<CharacterInstance>()->GetCharacterStats());

	Ability* ability = &myAbilityData->GetAbility(entity->GetComponent<AbilityInstance>()->GetAbilityID());

	entity->AddComponent<Mesh>()->Init(entity);
	entity->GetComponent<Mesh>()->SetUpModel(ability->fbxPath);
	entity->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
	entity->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);
	entity->SetRotation(V3F());
	entity->AddComponent<LifeTime>()->Init(entity);
	entity->GetComponent<LifeTime>()->SetLifeTime(ability->duration);

	entity->AddComponent<Collision>()->Init(entity);

	entity->AddComponent<Audio>()->Init(entity);
	//entity->GetComponent<Audio>()->InitEventIDs();

	entity->GetComponent<Collision>()->SetIsFriendly(false);
	entity->GetComponent<Collision>()->SetDamageToOthers(myAbilityData->GetAbility(aAbilityType).damageMin);

	entity->GetComponent<Collision>()->SetCollisionRadius(ability->radius);
	entity->GetComponent<Collision>()->SetHeightOffset(50.0f);
	entity->Spawn(aPosition);

	myEntityVector->push_back(entity);

	if (ability->isAnimated == true)
	{
		Entity* entityVFX = myEntityPoolPtr->Retrieve();
		entityVFX->Init(EntityType::Ability, (*myEntityIDInt)++);

		entityVFX->AddComponent<Mesh>()->Init(entityVFX);
		entityVFX->GetComponent<Mesh>()->SetUpModel(ability->animationPath);
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetScale(V3F(1.0f, 1.0f, 1.0f));
		entityVFX->GetComponent<Mesh>()->GetModelInstance()->SetExpectedLifeTime(ability->duration);
		entityVFX->AddComponent<AnimationComponent>()->Init(entityVFX);

		entityVFX->AddComponent<LifeTime>()->Init(entityVFX);
		entityVFX->GetComponent<LifeTime>()->SetLifeTime(ability->duration);

		entityVFX->SetRotation(V3F());
		entityVFX->Spawn(aPosition);
		myAbilityVFXVector->push_back(entityVFX);
	}
}

void AbilityFactory::SetPlayerPtr(Entity* aPlayerPtr)
{
	myPlayerPtr = aPlayerPtr;
}

void AbilityFactory::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::SpawnAbility)
	{
		CreateAbility(CAST(Entity*, aMessage.myData), V3F(aMessage.myFloatValue, 0.0f, aMessage.myFloatValue2), aMessage.myIntValue);
	}
	else if (aMessage.myMessageType == MessageType::SpawnAbilityAt)
	{
		CreateAbility(CAST(Entity*, aMessage.myData), V3F(aMessage.myFloatValue, aMessage.myIntValue2, aMessage.myFloatValue2), aMessage.myIntValue, aMessage.myBool);
	}
	
}
