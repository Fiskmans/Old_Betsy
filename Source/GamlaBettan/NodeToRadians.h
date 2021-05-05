#pragma once
#include "CNodeType.h"
class ToRadians :
	public CNodeType
{
public:
	ToRadians();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "ToRadians"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

