#pragma once
#include "CNodeType.h"
class Sin :
	public CNodeType
{
public:
	Sin();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Sin"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

