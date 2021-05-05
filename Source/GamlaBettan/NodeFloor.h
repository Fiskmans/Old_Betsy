#pragma once
#include "CNodeType.h"
class Floor :
	public CNodeType
{
public:
	Floor();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Floor"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

