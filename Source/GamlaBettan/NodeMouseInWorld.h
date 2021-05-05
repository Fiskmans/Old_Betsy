#pragma once
#include "CNodeType.h"
class MouseInWorld :
	public CNodeType
{
public:
	MouseInWorld();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Mouse In World"; }
	virtual std::string GetNodeTypeCategory() override { return "Input"; }
};

