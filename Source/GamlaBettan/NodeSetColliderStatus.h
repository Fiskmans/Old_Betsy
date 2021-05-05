#pragma once
#include "CNodeType.h"
class NodeSetColliderStatus :
	public CNodeType
{
public:
	NodeSetColliderStatus();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SetColliderStatus"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

