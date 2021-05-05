#include "pch.h"
#include "AIPollingStation.h"

AIPollingStation::AIPollingStation()
{
}

AIPollingStation::~AIPollingStation()
{
}

void AIPollingStation::AddSeekingEnemy(Entity* aEntity)
{
	for (int i = 0; i < mySeekingEnemies.size(); ++i)
	{
		if (mySeekingEnemies[i] == aEntity)
		{
			//Already excist in container
			return;
		}
	}
	mySeekingEnemies.push_back(aEntity);
}

void AIPollingStation::RemoveSeekingEnemy(Entity* aEntity)
{
	for (int i = 0; i < mySeekingEnemies.size(); ++i)
	{
		if (mySeekingEnemies[i] == aEntity)
		{
			mySeekingEnemies.erase(mySeekingEnemies.begin() + i);
			break;
		}
	}
}


std::vector<Entity*> AIPollingStation::GetSeekingEnemies()
{
	return mySeekingEnemies;
}

void AIPollingStation::ClearSeekingEnemies()
{
	mySeekingEnemies.clear();
}

void AIPollingStation::SetPlayer(Entity* aPlayer)
{
	myPlayer = aPlayer;
}

Entity* AIPollingStation::GetPlayer()
{
	return myPlayer;
}
