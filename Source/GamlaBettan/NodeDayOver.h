#pragma once
#include "CNodeType.h"
#include "Observer.hpp"

class NodeDayOver: public CNodeType, public Observer
{
public:
	NodeDayOver();
	~NodeDayOver();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "Day over"; }
	virtual std::string GetNodeTypeCategory() override { return "Interactables"; }

private:
	virtual void RecieveMessage(const Message& aMessage) override;

	std::vector<CNodeInstance*> myNodes;
};
