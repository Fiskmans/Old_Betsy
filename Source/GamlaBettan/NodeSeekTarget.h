#pragma once
#include "CNodeType.h"
class NodeSeekTarget :
	public CNodeType
{
public:
	NodeSeekTarget();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Seek Target"; }
	virtual std::string GetNodeTypeCategory() override { return "AI"; }
};
