#include "pch.h"
#include "AbilityInstance.h"
#include "AbilityFactory.h"

AbilityInstance::AbilityInstance() :
	myCastingEntity(nullptr),
	myTickRateTimer(0),
	myColliderType(AbilityColliderType::None),
	myIsColliderActive(false)
{
}

AbilityInstance::~AbilityInstance()
{
}

void AbilityInstance::Init(Entity* aEntity)
{
	aEntity;
}

void AbilityInstance::Init(Entity* aEntity, Ability* aAbility, Entity* aCastingEntity, Stats* aStats)
{
	aStats;

	myEntity = aEntity;
	myAbility = *aAbility;
	myCastingEntity = aCastingEntity;
	myCastPointTimer = myAbility.castPoint;
	myTickRateTimer = myAbility.tickRate;
	//FIX COLLIDER TYPE
	myColliderType = AbilityColliderType::Box;
	//myDamagedEntities.push_back(100000);
}

void AbilityInstance::Update(const float aDeltaTime)
{
	if (myAbility.tickRate > 0.0f)
	{
		myTickRateTimer -= aDeltaTime;
		if (myTickRateTimer <= 0.0f)
		{
			myTickRateTimer = myAbility.tickRate;
			myDamagedEntities.clear();
		}
	}

	if (myCastPointTimer > 0.0f)
	{
		myCastPointTimer -= aDeltaTime;
		if (myCastPointTimer <= 0.0f)
		{
			myIsColliderActive = true;

			//myEntity->GetComponent<Audio>()->PlayAudioEvent(AudioEvent::Activated);
		}
	}

	if (myIsColliderActive)
	{
		myCastPointTimer -= aDeltaTime;
		if (myCastPointTimer < -0.3f)
		{
			if (myAbility.abilityID != 3)
			{
				myIsColliderActive = false;
			}
		}
	}
	
	if (myAbility.abilityID == 86)
	{
		myEntity->SetRotation(myCastingEntity->GetRotation());
	}
}

void AbilityInstance::Reset()
{
	myEntity = nullptr;
	myCastingEntity = nullptr;
	myDamagedEntities.clear();
	myIsColliderActive = false;
}

bool AbilityInstance::CanDamageEntity(Entity* aEntity)
{
	if (aEntity->GetEntityType() == EntityType::Enemy || aEntity->GetEntityType() == EntityType::Player)
	{
		int entityID = aEntity->GetEntityID();

		if (myAbility.maxNrOfTargets > 0)
		{
			if (myDamagedEntities.size() >= myAbility.maxNrOfTargets)
			{
				return false;
			}
		}
		for (int index = 0; index < myDamagedEntities.size(); index++)
		{
			if (myDamagedEntities[index] == entityID)
			{
				return false;
			}
		}
		myDamagedEntities.push_back(entityID);
	}

	return true;
}

int AbilityInstance::GetAbilityID()
{
	return myAbility.abilityID;
}

int AbilityInstance::GetRageCost()
{
	return CAST(int, myAbility.rageCost);
}

Ability* AbilityInstance::GetAbility()
{
	return &myAbility;
}

Entity* AbilityInstance::GetCastingEntity()
{
	return myCastingEntity;
}

void AbilityInstance::SetIsColliderActive(bool aVal)
{
	myIsColliderActive = aVal;
}

bool AbilityInstance::GetIsColliderActive()
{
	return myIsColliderActive;
}

void AbilityInstance::OnAttach()
{
}

void AbilityInstance::OnDetach()
{
}

void AbilityInstance::OnKillMe()
{
}
