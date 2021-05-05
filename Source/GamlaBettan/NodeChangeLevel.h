#pragma once
#include "CNodeType.h"
#include "Publisher.hpp"

class NodeChangeLevel : public CNodeType, public Publisher
{
public:
	NodeChangeLevel();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "ChangeLevel"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }

private:

};

