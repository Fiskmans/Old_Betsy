#pragma once
#include "CNodeType.h"
class NodeSetTargetPosition :
	public CNodeType
{
public:
	NodeSetTargetPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SetTargetPosition"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

