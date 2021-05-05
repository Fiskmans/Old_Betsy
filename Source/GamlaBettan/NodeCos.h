#pragma once
#include "CNodeType.h"
class Cos :
	public CNodeType
{
public:
	Cos();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Cos"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

