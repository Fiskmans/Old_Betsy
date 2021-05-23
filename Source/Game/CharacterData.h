#pragma once
#include <unordered_map>

struct Stats
{
	std::string name = "Name not loaded";
	int characterID = 0;
	std::string fbxPath = "";
	float modelScale = 0;
	int height = 0;

	//CHANGES WITH DIFFICULTY
	int life = 0;
	float damageMultiplier = 0;
	float xpWorth = 0;

	//STATIC PER CHARACTER TYPE
	float collisionRadius = 0;
	float movementSpeed = 0;
};

class CharacterData
{
public:
	void Load();
	Stats* GetCharacterStats(int aCharacterID);

private:
	std::unordered_map<int, Stats*> myCharacterTypes;
	Stats* myfakeStats = nullptr;
};

