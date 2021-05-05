#include "pch.h"
#include "LifeTime.h"
#include "Entity.h"
#include "Audio.h"

LifeTime::LifeTime() :
	myCurrentLifeTime(0.0f),
	mySetLifeTime(0.0f)
{
}

LifeTime::~LifeTime()
{
}

void LifeTime::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myCurrentLifeTime = 10.0f;
	mySetLifeTime = myCurrentLifeTime;
}

void LifeTime::Update(const float aDeltaTime)
{
	myCurrentLifeTime -= aDeltaTime;
	if (myCurrentLifeTime <= 0.0f)
	{
		myEntity->SetIsAlive(false);
	}
}

void LifeTime::Reset()
{
	myCurrentLifeTime = 0.0f;
	mySetLifeTime = 0.0f;
	myEntity = nullptr;
}

void LifeTime::SetLifeTime(const float aTime)
{
	myCurrentLifeTime = aTime;
	mySetLifeTime = aTime;
}

void LifeTime::OnAttach()
{
}

void LifeTime::OnDetach()
{
}

void LifeTime::OnKillMe()
{
}
