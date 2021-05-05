#pragma once
#include <unordered_map>
#include <string>


struct Ability
{
	int abilityID = 0;
	std::string name = "default";
	std::string fbxPath = "Data/Models/PlayerAttack1FX/playerAttack1FX.fbx";
	bool isAnimated = false;
	std::string animationPath = "Data/PATHPATH";
	float rageCost = 0.0f;
	float rangeSqrd = 4.0f;
	float damageMin = 10.0f;
	float damageMax = 14.0f;
	float critChance = 0.05f;
	float critDamage = 2.0f;
	float tickRate = 0.0f;
	std::string colliderType = "Box";
	float width = 100.0f;
	float length = 100.0f;
	float radius = 0.0f;
	float angle = 0.0f;
	float animationTime = 0.5f;
	float castPoint = 0.1f;
	float duration = 0.2f;
	float cooldown = 0.5f;
	int maxNrOfTargets = 0;
};



class AbilityData
{
public:
	void Load();
	Ability& GetAbility(int aAbilityID);

private:
	std::unordered_map<int, Ability*> myAbilityTypes;
};