#pragma once
#include "CNodeType.h"
class GetObjectPos :
	public CNodeType
{
public:
	GetObjectPos();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "GetObjectPosition"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

