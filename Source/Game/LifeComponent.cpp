#include "pch.pch"
#include "Life.h"
#include "Entity.h"

Life::Life() :
	myLife(1000)
{
	myComponentType = ComponentType::Life;
}

Life::Life(Entity* aEntity, unsigned int aLives) :
	myLife(aLives)
{
	myEntity = aEntity;
	myComponentType = ComponentType::Life;
}

Life::~Life()
{
}

void Life::Init(Entity* aEntity)
{
}

void Life::Update(const float aDeltaTime)
{
}

void Life::Reset()
{
	myLife = 10;
}

void Life::TakeDamage(int aAmount)
{
	myLife -= aAmount;
	if (myLife < 1)
	{
		myLife = 0;
		myEntity->SetIsAlive(false);
	}

}
