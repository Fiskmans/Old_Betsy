#pragma once
#include "CNodeType.h"
class LengthSqr :
	public CNodeType
{
public:
	LengthSqr();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "LengthSqr"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

