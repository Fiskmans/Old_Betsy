#include "pch.h"
#include "NodeForeach.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"

Foreach::Foreach()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("OnIter", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	myPins.push_back(CPin("Filter", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<GameObjectId>(3);
	SetPinType<EntityType>(4);
}

int Foreach::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	EntityType filter;
	if (!aTriggeringNodeInstance->ReadData(4, filter)) { return -1; }
	std::vector<class Entity*> entities = aTriggeringNodeInstance->ourPollingStation->GetFiltered(filter);

	for (auto& i : entities)
	{
		GameObjectId id = i;
		aTriggeringNodeInstance->WriteData(3, id);
		aTriggeringNodeInstance->ExitVia(2);
	}

	return 1;
}
