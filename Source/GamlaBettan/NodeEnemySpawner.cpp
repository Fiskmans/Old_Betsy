#include "pch.h"
#include "NodeEnemySpawner.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "..//Game/DataStructs.h"
#include "..//Game/EnemyFactory.h"

NodeEnemySpawner::NodeEnemySpawner() :
	myEnemyFactory(nullptr)
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("CharacterID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Amount", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("SpawnIntervalMin", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("SpawnIntervalMax", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Position", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Radius", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("TargetPosition", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("TargetRadius", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("TargetPlayer", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("SpawnFalling", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("SpawnSleeping", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<int>(2);
	SetPinType<int>(3);
	SetPinType<float>(4);
	SetPinType<float>(5);
	SetPinType<V3F>(6);
	SetPinType<float>(7);
	SetPinType<V3F>(8);
	SetPinType<float>(9);
	SetPinType<bool>(10);
	SetPinType<bool>(11);
	SetPinType<bool>(12);
}

int NodeEnemySpawner::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int charID;
	int amount;
	float intervalMin;
	float intervalMax;
	V3F position;
	float radius;
	V3F targetPos = {0, 0, 0};
	float targetRadius = 0.f;
	bool targetPlayer = false;
	bool fall = false;
	bool sleep = false;

	if (!aTriggeringNodeInstance->ReadData(2, charID)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, amount)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(4, intervalMin)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(5, intervalMax)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(6, position)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(7, radius)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(10, targetPlayer)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(11, fall)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(12, fall)) { return -1; }
	if (!targetPlayer && !aTriggeringNodeInstance->ReadData(8, targetPos)) { return -1; }
	if (!targetPlayer && !aTriggeringNodeInstance->ReadData(9, targetRadius)) { return -1; }

	myEnemyFactory = aTriggeringNodeInstance->ourPollingStation->GetEnemyFactory();
	myEnemyFactory->QueueSpawn(charID, amount, intervalMin, intervalMax, position, radius, targetPos, targetRadius, targetPlayer, fall, false, sleep);

	return 1;
}
