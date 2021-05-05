#pragma once
#include "CNodeType.h"
class Lerp :
	public CNodeType
{
public:
	Lerp();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Lerp"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

