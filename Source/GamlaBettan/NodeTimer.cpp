#include "pch.h"
#include "NodeTimer.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "Timer.hpp"
#include "Time.h"
#include "TimerController.h"

NodeTimer::NodeTimer()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Flow));
	myPins.push_back(CPin("On Trigger", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("Time", CPin::PinTypeInOut::PinTypeInOut_IN,  CPin::PinType::Data));
	myPins.push_back(CPin("Repeating", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Interval", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Timer Id", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));

	SetPinType<float>(3);
	SetPinType<bool>(4);
	SetPinType<float>(5);
	SetPinType<TimerId>(6);
}

int NodeTimer::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	float time;
	if (!aTriggeringNodeInstance->ReadData(3, time)) { return -1; }

	bool repeating;
	if (!aTriggeringNodeInstance->ReadData(4, repeating)) { return -1; }

	TimerController* timerController = aTriggeringNodeInstance->ourPollingStation->GetTimerController();
	if (timerController)
	{
		TimerId id;
		if (repeating)
		{
			float interval;
			if (!aTriggeringNodeInstance->ReadData(5, interval)) { return -1; }
			id = timerController->AddCallback([aTriggeringNodeInstance]()->void { aTriggeringNodeInstance->ExitVia(2); }, time, interval);
		}
		else
		{
			id = timerController->AddCallback([aTriggeringNodeInstance]()->void { aTriggeringNodeInstance->ExitVia(2); }, time);
		}
		aTriggeringNodeInstance->WriteData(CAST(unsigned int, id), 6);
	}
	return 1;
}
