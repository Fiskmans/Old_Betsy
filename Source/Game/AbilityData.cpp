#include "pch.h"
#include "AbilityData.h"
#pragma warning(push)
#pragma warning(disable:26451)
#pragma warning(disable:26812)
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

void AbilityData::Load()
{
	rapidjson::Document abilitiesDoc;

#pragma warning(suppress : 4996)
	FILE* fp = fopen("Data\\Metrics\\Abilities.json", "rb");
	char readBuffer[4096];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	abilitiesDoc.ParseStream(is);
	fclose(fp);

	for (int i = 0; i < int(abilitiesDoc["Abilities"].GetArray().Size()); ++i)
	{
		Ability* ability = new Ability();

		ability->name = abilitiesDoc["Abilities"][i]["name"].GetString();
		ability->abilityID = abilitiesDoc["Abilities"][i]["abilityID"].GetInt();
		ability->fbxPath = abilitiesDoc["Abilities"][i]["fbxPath"].GetString();
		ability->isAnimated = abilitiesDoc["Abilities"][i]["isAnimated"].GetBool();
		ability->animationPath = abilitiesDoc["Abilities"][i]["animationPath"].GetString();

		ability->rageCost = float(abilitiesDoc["Abilities"][i]["rageCost"].GetInt());
		float range = abilitiesDoc["Abilities"][i]["range"].GetFloat();
		ability->rangeSqrd = range*range;
		
		ability->damageMin = abilitiesDoc["Abilities"][i]["damageMin"].GetFloat();
		ability->damageMax = abilitiesDoc["Abilities"][i]["damageMax"].GetFloat();

		ability->critChance = abilitiesDoc["Abilities"][i]["critChance"].GetFloat();
		ability->critDamage = abilitiesDoc["Abilities"][i]["critDamageMult"].GetFloat();

		ability->tickRate = abilitiesDoc["Abilities"][i]["tickRate"].GetFloat();
		
		ability->colliderType = abilitiesDoc["Abilities"][i]["colliderType"].GetString();
		ability->width = abilitiesDoc["Abilities"][i]["width"].GetFloat();
		ability->length = abilitiesDoc["Abilities"][i]["length"].GetFloat();
		ability->radius = abilitiesDoc["Abilities"][i]["radius"].GetFloat();
		ability->angle = abilitiesDoc["Abilities"][i]["angle"].GetFloat();

		ability->animationTime = abilitiesDoc["Abilities"][i]["animationTime"].GetFloat();
		ability->castPoint = abilitiesDoc["Abilities"][i]["castPoint"].GetFloat();
		ability->duration = abilitiesDoc["Abilities"][i]["duration"].GetFloat();
		ability->cooldown = abilitiesDoc["Abilities"][i]["cooldown"].GetFloat();
		ability->maxNrOfTargets = abilitiesDoc["Abilities"][i]["maxnrtargets"].GetInt();


		myAbilityTypes[ability->abilityID] = ability;
	}
}

Ability& AbilityData::GetAbility(int aAbilityID)
{
	if (myAbilityTypes.count(aAbilityID))
	{
		return *myAbilityTypes[aAbilityID];
	}
	else
	{
		SYSERROR("GET ABILITY CALLED WITH INVALID ABILITY ID IN ABILITYDATA","");
		return *myAbilityTypes[0];
	}

	//static Ability t;
	//return t;
}

#pragma warning(pop)