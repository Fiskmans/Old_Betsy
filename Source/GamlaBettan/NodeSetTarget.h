#pragma once
#include "CNodeType.h"
class NodeSetTarget :
	public CNodeType
{
public:
	NodeSetTarget();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Set Target"; }
	virtual std::string GetNodeTypeCategory() override { return "AI"; }
};