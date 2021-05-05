#pragma once
#include "CNodeType.h"
class Abs :
	public CNodeType
{
public:
	Abs();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Abs"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

