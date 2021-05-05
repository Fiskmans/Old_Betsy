#include "pch.h"
#include "NodeGetPlayerID.h"
#include "CNodeInstance.h"
#include "../Game/Entity.h"

GetPlayerID::GetPlayerID()
{
	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<GameObjectId>(0);
}

int GetPlayerID::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	Entity* player = aTriggeringNodeInstance->ourPollingStation->GetPlayer();
	if (!player)
	{
		return -1;
	}

	
	aTriggeringNodeInstance->WriteData(0, GameObjectId(player));

	return -1;
}
