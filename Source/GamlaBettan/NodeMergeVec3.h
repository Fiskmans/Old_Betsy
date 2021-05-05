#pragma once
#include "CNodeType.h"
class MergeVec3 :
	public CNodeType
{
public:
	MergeVec3();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "MergeVec3"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

