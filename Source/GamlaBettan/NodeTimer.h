#pragma once
#include "CNodeType.h"
class NodeTimer :
	public CNodeType
{
public:
	NodeTimer();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Timer"; }
	virtual std::string GetNodeTypeCategory() override { return "Flow"; }
};

