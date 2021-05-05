#pragma once
#include "CNodeType.h"
class ForEachEnemyWithinRange :
	public CNodeType
{
public:
	ForEachEnemyWithinRange();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Foreach Enemy Within Range"; }
	virtual std::string GetNodeTypeCategory() override { return "AI"; }
};
