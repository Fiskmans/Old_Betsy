#pragma once
#include "CNodeType.h"
#include "Observer.hpp"

class NodeInteractWith :
	public CNodeType, public Observer
{
public:
	NodeInteractWith();
	~NodeInteractWith();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Interact with"; }
	virtual std::string GetNodeTypeCategory() override { return "Interactables"; }
	bool IsStartNode() override { return true; }

private:
	virtual void RecieveMessage(const Message& aMessage) override;

	std::unordered_map<unsigned int, CNodeInstance*> myData;
};

