#include "pch.h"
#include "NodeSpawnAbility.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "..//Game/DataStructs.h"

NodeSpawnAbility::NodeSpawnAbility() :
	myAbilityFactory(nullptr)
{
	myPins.push_back(CPin("In !!!This node is deprecated!!!", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("AbilityID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Position", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Scale", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("CreatedAbility", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));

	SetPinType<int>(2);
	SetPinType<V3F>(3);
	SetPinType<V3F>(4);
	SetPinType<GameObjectId>(5);
}

int NodeSpawnAbility::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int abilityID;
	V3F position;
	V3F scale;
	if (!aTriggeringNodeInstance->ReadData(2, abilityID)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, position)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(4, scale)) { return -1; }

	//myAbilityFactory = aTriggeringNodeInstance->ourPollingStation->GetAbilityFactory();
	//Entity* ability = myAbilityFactory->CreateAbility(position, scale, abilityID);
	//if (ability)
	//{
	//	aTriggeringNodeInstance->WriteData(5, GameObjectId(ability));
	//}
	return 1;
}
