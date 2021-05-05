#pragma once
#include "CNodeType.h"
class Atan :
	public CNodeType
{
public:
	Atan();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Atan"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

