#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"
#include "Observer.hpp"
#include "..//Game/Enums.h"

enum class ButtonInput
{
	Reload = ENUM_CAST(MessageType::InputReload),
	Jump = ENUM_CAST(MessageType::InputJumpHit),
	Pause = ENUM_CAST(MessageType::InputPauseHit),
	Unpause = ENUM_CAST(MessageType::InputUnPauseHit),
	Crouch = ENUM_CAST(MessageType::InputCrouchHit),
	Run = ENUM_CAST(MessageType::InputRunHit),
	LeftMouse = ENUM_CAST(MessageType::InputLeftMouseHit),
	RightMouse = ENUM_CAST(MessageType::InputRightMouseHit),
	Interact = ENUM_CAST(MessageType::InputInteractHit),
	None
};

class NodeOnButtonHit:
	public CNodeType, public Observer
{
public:
	NodeOnButtonHit();
	~NodeOnButtonHit();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia) override;
	bool IsStartNode() override { return true; }
	virtual std::string GetNodeName() { return "OnButtonHit"; }
	virtual std::string GetNodeTypeCategory() override { return "Game Control"; }

private:
	virtual void RecieveMessage(const Message& aMessage) override;

	CNodeInstance* myTriggeringInstance;

	void SubToMsg();
	void UnsubToMsg();
};