#pragma once
#include "CNodeType.h"
class NodeGreater :
	public CNodeType
{
public:
	NodeGreater();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Greater"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

