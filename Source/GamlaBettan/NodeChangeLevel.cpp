#include "pch.h"
#include "NodeChangeLevel.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"

NodeChangeLevel::NodeChangeLevel()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("SpecificLevelInsteadOfNext", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("LevelID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<bool>(1);
	SetPinType<int>(2);
}

int NodeChangeLevel::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	bool goToSpecificLevel;
	if (!aTriggeringNodeInstance->ReadData(1, goToSpecificLevel))
	{
		SYSERROR("bool pin broken on NodeChangeLevel");
		return -1;
	}

	if (goToSpecificLevel)
	{
		int levelID = 0;
		if (!aTriggeringNodeInstance->ReadData(2, levelID))
		{
			SYSERROR("LEVEL ID Not correct on pin in NodeChangeLevel");
			return -1;
		}

		Message changeToSpecificLevelMessage;
		changeToSpecificLevelMessage.myMessageType = MessageType::ChangeLevel;
		changeToSpecificLevelMessage.myIntValue = levelID;
		SendMessages(changeToSpecificLevelMessage);
	}
	else
	{
		Message nextLevelMessage;
		nextLevelMessage.myMessageType = MessageType::NextLevel;
		SendMessages(nextLevelMessage);
	}
	return -1;
}
