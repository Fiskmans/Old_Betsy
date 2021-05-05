#include "pch.h"
#include "Aura.h"


Aura::Aura() : 
	myDuration(10.0f),
	myShouldBeDisplayed(true)
{
}

Aura::~Aura()
{
}

void Aura::Init(float aDuration, Entity* aTargetedEntity)
{
}

void Aura::Update(float aDeltaTime)
{
}

void Aura::SetDuration(float aDuration)
{
	myDuration = aDuration;
}

float Aura::GetDuration()
{
	return myDuration;
}

bool Aura::GetShouldBeDisplayed()
{
	return myShouldBeDisplayed;
}

AuraType Aura::GetAuraType()
{
	return myAuraType;
}
