#pragma once
#include "CNodeType.h"
class NodeLoadLevel :
	public CNodeType
{
public:
	NodeLoadLevel();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Load Level"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

