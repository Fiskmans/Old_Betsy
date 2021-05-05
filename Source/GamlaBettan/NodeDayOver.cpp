#include "pch.h"
#include "NodeDayOver.h"
#include "CNodeInstance.h"

NodeDayOver::NodeDayOver()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	SubscribeToMessage(MessageType::NewDay);
}

NodeDayOver::~NodeDayOver()
{
	UnSubscribeToMessage(MessageType::NewDay);
}

int NodeDayOver::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	myNodes.push_back(aTriggeringNodeInstance);

	return -1;
}

void NodeDayOver::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::NewDay)
	{
		for (auto& i : myNodes)
		{
			i->ExitVia(1);
		}

		myNodes.clear();
	}
}
