#pragma once
#include "CNodeType.h"
class NodeSetToIdle :
	public CNodeType
{
public:
	NodeSetToIdle();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Set To Idle"; }
	virtual std::string GetNodeTypeCategory() override { return "AI"; }
};
