#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"
#include "Observer.hpp"
#include "..//Game/Enums.h"

struct TriggerStruct
{
	CNodeInstance* aNodeInstance = nullptr;
	bool aEnterOnce = false;
	bool aFirstEnter = true;
	bool aExitOnce = false;
	bool aFirstExit = true;
	EntityType aFilteredTypeID = EntityType::None;
};

class NodeOnTrigger :
	public CNodeType, public Observer
{
public:
	NodeOnTrigger();
	~NodeOnTrigger();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	bool IsStartNode() override { return true; }
	virtual std::string GetNodeName() { return "OnTriggerEnter"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }

private:
	virtual void RecieveMessage(const Message& aMessage) override;
	
	std::unordered_map<int, TriggerStruct*> myTriggerStructs;
};