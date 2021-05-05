#pragma once
#include "CNodeType.h"
#include "Publisher.hpp"

class EnemyFactory;

class ContinuousSpawner :
	public CNodeType, public Publisher
{
public:
	ContinuousSpawner();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Continuous spawner"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }

private:
	EnemyFactory* myEnemyFactory;
	bool* myIsActivePtr;
	std::unordered_map<CNodeInstance*, bool*> myBoolPtrs;
};

