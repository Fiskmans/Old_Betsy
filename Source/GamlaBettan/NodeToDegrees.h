#pragma once
#include "CNodeType.h"
class ToDegrees :
	public CNodeType
{
public:
	ToDegrees();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "ToDegrees"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

