#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeStart : public CNodeType
{
public:
	CNodeTypeStart();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	bool IsStartNode() override { return true; }
	virtual std::string GetNodeName() { return "Start"; }
	
};

