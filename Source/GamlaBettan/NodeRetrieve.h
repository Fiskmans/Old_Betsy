#pragma once
#include "CNodeType.h"
class NodeRetrieve :
	public CNodeType
{
public:
	NodeRetrieve();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Retrieve"; }
	virtual std::string GetNodeTypeCategory() override { return "Utility"; }
};

