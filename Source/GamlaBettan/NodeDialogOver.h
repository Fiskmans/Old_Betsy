#pragma once
#include "CNodeType.h"
#include <unordered_map>
#include "Observer.hpp"

class NodeDialogOver :
	public CNodeType, public Observer
{
public:
	NodeDialogOver();
	~NodeDialogOver();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Dialog over"; }
	virtual std::string GetNodeTypeCategory() override { return "Interactables"; }
	bool IsStartNode() override { return true; }

private:
	virtual void RecieveMessage(const Message& aMessage) override;

	std::unordered_map<std::string, CNodeInstance*> myData;

	CNodeInstance* myQueuedNode;
};

