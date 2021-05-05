#pragma once
#include "CNodeType.h"
class NodeStartCinematic :
	public CNodeType
{
public:
	NodeStartCinematic();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "StartCinematic"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};

