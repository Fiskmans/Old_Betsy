#pragma once
#include "CNodeType.h"
class SetObjectPosition :
	public CNodeType
{
public:
	SetObjectPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SetObjectPosition"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

