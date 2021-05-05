#pragma once
#include "CNodeType.h"
class If :
	public CNodeType
{
public:
	If();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "If"; }
	virtual std::string GetNodeTypeCategory() override { return "Flow"; }
};

