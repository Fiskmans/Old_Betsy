#pragma once
#include "CNodeType.h"
class Sub :
	public CNodeType
{
public:
	Sub();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Sub"; }
	virtual std::string GetNodeTypeCategory() override { return "Math"; }
};

