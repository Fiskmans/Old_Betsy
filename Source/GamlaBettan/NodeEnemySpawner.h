#pragma once
#include "CNodeType.h"
#include "Publisher.hpp"

class EnemyFactory;

class NodeEnemySpawner :
	public CNodeType, public Publisher
{
public:
	NodeEnemySpawner();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Enemy spawner"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }

private:
	EnemyFactory* myEnemyFactory;
};

