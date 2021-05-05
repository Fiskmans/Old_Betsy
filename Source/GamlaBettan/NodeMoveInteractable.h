#pragma once
#include "NodeChangeInteractableMesh.h"
#include "Observer.hpp"

class NodeMoveInteractable : public NodeChangeInteractableMesh
{
public:
	NodeMoveInteractable();
	virtual int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Move interactable"; }
	virtual std::string GetNodeTypeCategory() override { return "Interactables"; }

private:
}; 
