#pragma once
#include "CNodeType.h"
class NodeGetMayaPos : public CNodeType
{
public:
	NodeGetMayaPos();
	~NodeGetMayaPos() = default;
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	virtual std::string GetNodeName() override { return "GetMayaPos"; }
	virtual std::string GetNodeTypeCategory() override { return "Utility"; }
};
