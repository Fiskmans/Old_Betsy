#pragma once
#include "CNodeType.h"
class NodeStopTimer :
	public CNodeType
{
public:
	NodeStopTimer();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "StopTimer"; }
	virtual std::string GetNodeTypeCategory() override { return "Flow"; }
};

