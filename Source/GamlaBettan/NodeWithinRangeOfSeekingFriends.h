#pragma once
#include "CNodeType.h"
class NodeWithinRangeOfSeekingFreinds :
	public CNodeType
{
public:
	NodeWithinRangeOfSeekingFreinds();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Within Range Of Seeking Friends"; }
	virtual std::string GetNodeTypeCategory() override { return "AI"; }
};
