#pragma once
#include "CNodeType.h"
class NodeAttackTarget :
	public CNodeType
{
public:
	NodeAttackTarget();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Attack Target"; }
	virtual std::string GetNodeTypeCategory() override { return "AI"; }
};
