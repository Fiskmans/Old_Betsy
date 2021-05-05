#pragma once
#include "CNodeType.h"
class Not :
	public CNodeType
{
public:
	Not();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Not"; }
	virtual std::string GetNodeTypeCategory() override { return "Logic"; }
};

