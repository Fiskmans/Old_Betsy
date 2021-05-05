#pragma once
#include "CNodeType.h"
class NodeStore :
	public CNodeType
{
public:
	NodeStore();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Store"; }
	virtual std::string GetNodeTypeCategory() override { return "Utility"; }
};

