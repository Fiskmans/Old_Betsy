#pragma once

class Entity;

class AIPollingStation
{
public:
	AIPollingStation();
	~AIPollingStation();

	void AddSeekingEnemy(Entity* aEntity);
	void RemoveSeekingEnemy(Entity* aEntity);
	std::vector<Entity*> GetSeekingEnemies();
	void ClearSeekingEnemies();
	void SetPlayer(Entity* aPlayer);
	Entity* GetPlayer();

private:
	std::vector<Entity*> mySeekingEnemies;
	Entity* myPlayer;
};