#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathAdd : public CNodeType
{
public:
	CNodeTypeMathAdd();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() { return "Add"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

