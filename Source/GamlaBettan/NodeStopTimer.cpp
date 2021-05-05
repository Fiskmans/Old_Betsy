#include "pch.h"
#include "NodeStopTimer.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "TimerController.h"

NodeStopTimer::NodeStopTimer()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("Timer Id", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	SetPinType<TimerId>(2);
}

int NodeStopTimer::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	TimerId input1;
	if (!aTriggeringNodeInstance->ReadData(2, input1)) { return -1; }

	aTriggeringNodeInstance->ourPollingStation->GetTimerController()->RemoveTimer(input1);
	return 1;
}
