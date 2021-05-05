#include "pch.h"
#include "CharacterInstance.h"
#include "AbilityData.h"
#include "Entity.h"

CharacterInstance::CharacterInstance()
{
	myStats = new Stats();
}

CharacterInstance::~CharacterInstance()
{
#ifndef  _RETAIL
	if (myEntity->GetEntityType() == EntityType::Player)
	{
		UnSubscribeToMessage(MessageType::InputAccept);
	}
#endif

	SAFE_DELETE(myStats);
}

void CharacterInstance::Init(Entity* aEntity)
{
}

void CharacterInstance::Init(Entity* aEntity, Stats* aCharacterStats)
{
	myEntity = aEntity;
	*myStats = *aCharacterStats;
}

void CharacterInstance::Update(const float aDeltaTime)
{

}

void CharacterInstance::Reset()
{
}

Stats* CharacterInstance::GetCharacterStats()
{
	return myStats;
}

const Stats* CharacterInstance::GetCharacterStats() const
{
	return myStats;
}


Entity* CharacterInstance::GetParentEntity()
{
	return myEntity;
}

void CharacterInstance::OnAttach()
{
}

void CharacterInstance::OnDetach()
{
}

void CharacterInstance::OnKillMe()
{
}

void CharacterInstance::RecieveMessage(const Message& aMessage)
{

}