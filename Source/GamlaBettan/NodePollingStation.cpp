#include "pch.h"
#include "NodePollingStation.h"
#include "../Game/Entity.h"
#include "..//Game/EnemyFactory.h"

void NodePollingStation::Reset()
{
	myEnemies = nullptr;
	myPlayer = nullptr;
	myMayaPositions.clear();
}

std::vector<class Entity*>& NodePollingStation::GetAllEntities()
{
	if (myEnemies)
	{
		return *myEnemies;
	}
	static std::vector<class Entity*> fail;
	return fail;
	// TODO: insert return statement here
}

std::vector<Entity*> NodePollingStation::GetFiltered(EntityType aFilter)
{
	std::vector<Entity*> out;
	/*if (myAllEntities)
	{
		for (auto& ent : *myAllEntities)
		{
			if (ent->GetEntityType() == aFilter)
			{
				out.push_back(ent);
			}
		}
	}*/

	if (aFilter == EntityType::Enemy)
	{
		return *myEnemies;
	}

	return out;
}

Entity* NodePollingStation::GetPlayer()
{
	return myPlayer;
}

EnemyFactory* NodePollingStation::GetEnemyFactory()
{
	return myEnemyFactory;
}

TimerController* NodePollingStation::GetTimerController()
{
	return myTimerController;
}

std::any NodePollingStation::GetSharedStorage(std::string aKey)
{
	return myStorage[aKey];
}

Scene* NodePollingStation::GetScene()
{
	return myScene;
}

ParticleFactory* NodePollingStation::GetParticleFactory()
{
	return myParticleFactory;
}

SpriteFactory* NodePollingStation::GetSpriteFactory()
{
	return mySpriteFactory;
}

V3F* NodePollingStation::GetMayaPos(const unsigned int anID)
{
	for (auto& p : myMayaPositions)
	{
		if (p.first == anID)
		{
			return &p.second;
		}
	}


	return nullptr;
}

std::vector<std::pair<unsigned int, V3F>>* NodePollingStation::GetMayaPositions()
{
	return &myMayaPositions;
}

void NodePollingStation::SetEnemyVector(std::vector<class Entity*>* aList)
{
	myEnemies = aList;
}

void NodePollingStation::SetPlayer(Entity* aPlayer)
{
	myPlayer = aPlayer;
}

void NodePollingStation::SetEnemyFactory(EnemyFactory* aEnemyFactory)
{
	myEnemyFactory = aEnemyFactory;
}

void NodePollingStation::SetTimerController(TimerController* aTimerController)
{
	myTimerController = aTimerController;
}

void NodePollingStation::SetSharedStorage(std::string aKey, std::any aValue)
{
	myStorage[aKey] = aValue;
}

void NodePollingStation::SetScene(Scene* aScene)
{
	myScene = aScene;
}

void NodePollingStation::SetParticleFactory(ParticleFactory* aParticleFactory)
{
	myParticleFactory = aParticleFactory;
}

void NodePollingStation::SetSpriteFactory(SpriteFactory* aSpriteFactory)
{
	mySpriteFactory = aSpriteFactory;
}

void NodePollingStation::ResetSharedStorage()
{
	myStorage.clear();
}

void NodePollingStation::AddMayaPos(unsigned int anID, const V3F& aPos)
{
	myMayaPositions.push_back({anID, aPos});
}
