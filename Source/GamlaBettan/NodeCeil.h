#pragma once
#include "CNodeType.h"
class Ceil :
	public CNodeType
{
public:
	Ceil();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Ceil"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

