#pragma once
#include "CNodeType.h"
class SplitVec3 :
	public CNodeType
{
public:
	SplitVec3();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "SplitVec3"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

