#pragma once
#include "CNodeType.h"
class NodeGetInteractable : public CNodeType
{
public:
	NodeGetInteractable();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Get interactable"; }
	virtual std::string GetNodeTypeCategory() override { return "Interactables"; }
};

