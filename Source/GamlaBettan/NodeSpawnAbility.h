#pragma once
#include "CNodeType.h"
#include "Publisher.hpp"

class AbilityFactory;

class NodeSpawnAbility :
	public CNodeType, public Publisher
{
public:
	NodeSpawnAbility();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SpawnAbility"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }

private:
	AbilityFactory* myAbilityFactory;
};