#pragma once
#include "CNodeType.h"
#include "Publisher.hpp"


class EnemyFactory;

class NodeSpawnEnemy :
	public CNodeType, public Publisher
{
public:
	NodeSpawnEnemy();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SpawnEnemy"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }

private:
	EnemyFactory* myEnemyFactory;
};

