#include "pch.h"
#include "NodeGetMayaPos.h"
#include "CNodeInstance.h"
#include "../Game/Entity.h"

NodeGetMayaPos::NodeGetMayaPos()
{
	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Pos", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	SetPinType<int>(0);
	SetPinType<V3F>(1);
}

int NodeGetMayaPos::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int ID;
	if (!aTriggeringNodeInstance->ReadData(0, ID)) { return -1; }

	V3F* pos = aTriggeringNodeInstance->ourPollingStation->GetMayaPos(ID);
	if (!pos)
	{
		SYSERROR("Maya position with ID:" + std::to_string(ID) + " not found!", "");
		return -1;
	}

	aTriggeringNodeInstance->WriteData(1, *pos);

	return -1;
}
