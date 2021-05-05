#include "pch.h"
#include "CharacterData.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

void CharacterData::Load()
{
	rapidjson::Document charactersDoc;

#pragma warning(suppress : 4996)
	FILE* fp = fopen("Data\\Metrics\\Characters.json", "rb");
	char readBuffer[4096];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	charactersDoc.ParseStream(is);
	fclose(fp);

	for (int i = 0; i < charactersDoc["Characters"].GetArray().Size(); ++i)
	{
		Stats* stats = new Stats();

		stats->name = charactersDoc["Characters"][i]["name"].GetString();
		stats->characterID = charactersDoc["Characters"][i]["characterID"].GetInt();
		stats->fbxPath = charactersDoc["Characters"][i]["fbxPath"].GetString();
		stats->modelScale = charactersDoc["Characters"][i]["modelScale"].GetFloat();
		stats->height = charactersDoc["Characters"][i]["height"].GetInt();
		
		stats->life = charactersDoc["Characters"][i]["life"].GetInt();
		
		stats->collisionRadius = charactersDoc["Characters"][i]["collisionRadius"].GetFloat();
		stats->movementSpeed = charactersDoc["Characters"][i]["movementSpeed"].GetFloat();

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
