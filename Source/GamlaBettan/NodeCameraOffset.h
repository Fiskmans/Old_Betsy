#pragma once
#include "CNodeType.h"
#include "Publisher.hpp"

class NodeCameraOffset : public Publisher,
	public CNodeType
{
public:
	NodeCameraOffset();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "CameraOffset"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }
};