#include "pch.h"
#include "NodeGetInteractable.h"
#include "CNodeInstance.h"

NodeGetInteractable::NodeGetInteractable()
{
	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<int>(0);
	SetPinType<GameObjectId>(1);
}

int NodeGetInteractable::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int ID;
	if (!aTriggeringNodeInstance->ReadData(0, ID)) { return -1; }

	for (auto& i : aTriggeringNodeInstance->ourPollingStation->GetAllEntities())
	{
		if (ID == i->myInteractableID)
		{
			aTriggeringNodeInstance->WriteData(1, GameObjectId(i));
			break;
		}
	}

	return -1;
}
