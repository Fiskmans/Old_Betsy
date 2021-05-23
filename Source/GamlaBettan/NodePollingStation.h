#pragma once
#include "../Game/Enums.h"
#include <any>
#include <unordered_map>
#include <string>

class Entity;
class EnemyFactory;
class AbilityFactory;
class TimerController;

class ModelLoader; 
class Scene;
class ParticleFactory;
class SpriteFactory;


class NodePollingStation
{
public:
	void Reset();

	//Access
	std::vector<Entity*>& GetAllEntities();
	std::vector<Entity*> GetFiltered(EntityType aFilter);
	Entity* GetPlayer();
	EnemyFactory* GetEnemyFactory();
	TimerController* GetTimerController();
	std::any GetSharedStorage(std::string aKey);
	Scene* GetScene();
	ParticleFactory* GetParticleFactory();
	SpriteFactory* GetSpriteFactory();
	V3F* GetMayaPos(const unsigned int anID);
	std::vector<std::pair<unsigned int, V3F>>* GetMayaPositions();

	//Setters
	void SetEnemyVector(std::vector<Entity*>* aList);
	void SetPlayer(Entity* aPlayer);
	void SetEnemyFactory(EnemyFactory* aEnemyFactory);
	void SetTimerController(TimerController* aTimerController);
	void SetSharedStorage(std::string aKey,std::any aValue);

	void SetScene(Scene* aScene);
	void SetParticleFactory(ParticleFactory* aParticleFactory);
	void SetSpriteFactory(SpriteFactory* aSpriteFactory);
	void ResetSharedStorage();
	void AddMayaPos(unsigned int anID, const V3F& aPos);

private:
	std::vector<std::pair<unsigned int, V3F>> myMayaPositions;
	std::unordered_map<std::string, std::any> myStorage;
	TimerController* myTimerController = nullptr;
	ParticleFactory* myParticleFactory = nullptr;
	std::vector<Entity*>* myEnemies = nullptr;
	SpriteFactory* mySpriteFactory = nullptr;
	EnemyFactory* myEnemyFactory = nullptr;
	Entity* myPlayer = nullptr;
	Scene* myScene = nullptr;
};

