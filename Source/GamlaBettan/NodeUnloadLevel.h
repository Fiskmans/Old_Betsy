#pragma once
#include "CNodeType.h"
class NodeUnloadLevel :
	public CNodeType
{
public:
	NodeUnloadLevel();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "UnloadLevel"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

