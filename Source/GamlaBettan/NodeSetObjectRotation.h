#pragma once
#include "CNodeType.h"
class NodeSetObjectRotation :
	public CNodeType
{
public:
	NodeSetObjectRotation();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SetObjectRotation"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};
