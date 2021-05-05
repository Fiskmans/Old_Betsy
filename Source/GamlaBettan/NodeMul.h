#pragma once
#include "CNodeType.h"
class Mul :
	public CNodeType
{
public:
	Mul();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Mul"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

