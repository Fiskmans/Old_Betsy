#include "pch.h"
#include "CharacterData.h"
#include "AssetManager.h"

void CharacterData::Load()
{
	FiskJSON::Object& root = AssetManager::GetInstance().GetJSON("metrics/Characters.json").GetAsJSON();

	for (auto& i : root["Characters"].Get<FiskJSON::Array>())
	{
		Stats* stats = new Stats();
		bool success = true;

		success &= (*i)["name"].GetIf(stats->name);
		success &= (*i)["characterID"].GetIf(stats->characterID);
		success &= (*i)["fbxPath"].GetIf(stats->fbxPath);
		success &= (*i)["modelScale"].GetIf(stats->modelScale);
		success &= (*i)["height"].GetIf(stats->height);
		
		success &= (*i)["life"].GetIf(stats->life);
		
		success &= (*i)["collisionRadius"].GetIf(stats->collisionRadius);
		success &= (*i)["movementSpeed"].GetIf(stats->movementSpeed);

		if (!success)
		{
			SYSERROR("Error in loading Character data ", stats->name);
			continue;
		}

		myCharacterTypes[stats->characterID] = stats;
	}
}

Stats* CharacterData::GetCharacterStats(int aCharacterID)
{
	if (myCharacterTypes.count(aCharacterID) > 0)
	{
		return myCharacterTypes[aCharacterID];
	}

	SYSERROR("Loaded enemy with fakestats as real stats weren't found in CharacterData", std::to_string(aCharacterID));
	return myfakeStats;
}
