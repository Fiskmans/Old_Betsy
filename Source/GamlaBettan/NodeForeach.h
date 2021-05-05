#pragma once
#include "CNodeType.h"
class Foreach :
	public CNodeType
{
public:
	Foreach();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Foreach"; }
	virtual std::string GetNodeTypeCategory() override { return "Flow"; }
};

