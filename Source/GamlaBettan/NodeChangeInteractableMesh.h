#pragma once
#include "CNodeType.h"
#include "Observer.hpp"

class NodeChangeInteractableMesh : public CNodeType
{
public:
	NodeChangeInteractableMesh();
	virtual int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Change interactable mesh"; }
	virtual std::string GetNodeTypeCategory() override { return "Interactables"; }

	static void SetGBPhysX(GBPhysX* aGBPhysX);

protected:
	static GBPhysX* ourGBPhysXPtr;
};