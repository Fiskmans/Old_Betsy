#pragma once
#include "CNodeType.h"
class GetPlayerID :
	public CNodeType
{
public:
	GetPlayerID();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "GetPlayerID"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};
