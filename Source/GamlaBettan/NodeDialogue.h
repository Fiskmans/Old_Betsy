#pragma once
#include "CNodeType.h"
#include "Observer.hpp"

class NodeDialogue : public CNodeType
{
public:
	NodeDialogue();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Dialogue"; }
	virtual std::string GetNodeTypeCategory() override { return "Interactables"; }

private:
};