#pragma once
#include "CNodeType.h"
class WithinRange :
	public CNodeType
{
public:
	WithinRange();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "WithinRange"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

