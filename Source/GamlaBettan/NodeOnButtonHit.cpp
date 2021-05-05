#include "pch.h"
#include "NodeOnButtonHit.h"
#include <iostream>
#include "CNodeType.h"
#include "CNodeInstance.h"

NodeOnButtonHit::NodeOnButtonHit()
{
	myPins.push_back(CPin("OnHit", CPin::PinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(CPin("Button", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<ButtonInput>(1);

	myTriggeringInstance = nullptr;

	SubToMsg();
}

NodeOnButtonHit::~NodeOnButtonHit()
{
	UnsubToMsg();
}

int NodeOnButtonHit::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	myTriggeringInstance = aTriggeringNodeInstance; //OOOPS there is only one instance do fixyyyy

	return -1;
}

void NodeOnButtonHit::RecieveMessage(const Message& aMessage)
{
	if (myTriggeringInstance)
	{
		ButtonInput input;
		myTriggeringInstance->ReadData(1, input);

		if (aMessage.myMessageType == CAST(MessageType, input))
		{
			myTriggeringInstance->ExitVia(0);
		}
	}
}

void NodeOnButtonHit::SubToMsg()
{
	SubscribeToMessage(CAST(MessageType, ButtonInput::Crouch));
	SubscribeToMessage(CAST(MessageType, ButtonInput::Interact));
	SubscribeToMessage(CAST(MessageType, ButtonInput::Jump));
	SubscribeToMessage(CAST(MessageType, ButtonInput::LeftMouse));
	SubscribeToMessage(CAST(MessageType, ButtonInput::Pause));
	SubscribeToMessage(CAST(MessageType, ButtonInput::Reload));
	SubscribeToMessage(CAST(MessageType, ButtonInput::RightMouse));
	SubscribeToMessage(CAST(MessageType, ButtonInput::Run));
	SubscribeToMessage(CAST(MessageType, ButtonInput::Unpause));
}

void NodeOnButtonHit::UnsubToMsg()
{
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::Crouch));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::Interact));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::Jump));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::LeftMouse));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::Pause));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::Reload));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::RightMouse));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::Run));
	UnSubscribeToMessage(CAST(MessageType, ButtonInput::Unpause));
}
