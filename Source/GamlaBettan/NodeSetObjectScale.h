#pragma once
#include "CNodeType.h"
class SetObjectScale :
	public CNodeType
{
public:
	SetObjectScale();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SetObjectScale"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

