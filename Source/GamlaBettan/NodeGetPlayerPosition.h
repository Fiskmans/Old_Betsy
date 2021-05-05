#pragma once
#include "CNodeType.h"
class GetPlayerPosition :
	public CNodeType
{
public:
	GetPlayerPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "GetPlayerPosition"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

