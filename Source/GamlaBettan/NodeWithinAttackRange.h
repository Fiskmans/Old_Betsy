#pragma once
#include "CNodeType.h"
class NodeWithinAttackRange :
	public CNodeType
{
public:
	NodeWithinAttackRange();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Within Attack Range"; }
	virtual std::string GetNodeTypeCategory() override { return "AI"; }

private:
	bool CheckAbilityRangeToFar(Entity* aCastingEntity, int aAbilityID, float aRangeSqrd);
	bool CheckAbilityRangeToClose(Entity* aCastingEntity, int aAbilityID, float aRangeSqrd, float closenessModifyer);
};