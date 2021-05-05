#include "pch.h"
#include "NodeContinuousSpawner.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "..//Game/DataStructs.h"
#include "..//Game/EnemyFactory.h"

ContinuousSpawner::ContinuousSpawner() :
	myEnemyFactory(nullptr),
	myIsActivePtr(nullptr)
{
	myPins.push_back(CPin("Start", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Stop", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
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

	SetPinType<int>(3);
	SetPinType<int>(4);
	SetPinType<float>(5);
	SetPinType<float>(6);
	SetPinType<V3F>(7);
	SetPinType<float>(8);
	SetPinType<V3F>(9);
	SetPinType<float>(10);
	SetPinType<bool>(11);
	SetPinType<bool>(12);
}

int ContinuousSpawner::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	if (aEnteredVia == 0)
	{
		int charID;
		int amount;
		float intervalMin;
		float intervalMax;
		V3F position;
		float radius;
		V3F targetPos = { 0, 0, 0 };
		float targetRadius = 0.f;
		bool targetPlayer = false;
		bool fall = false;

		if (!aTriggeringNodeInstance->ReadData(3, charID)) { return -1; }
		if (!aTriggeringNodeInstance->ReadData(4, amount)) { return -1; }
		if (!aTriggeringNodeInstance->ReadData(5, intervalMin)) { return -1; }
		if (!aTriggeringNodeInstance->ReadData(6, intervalMax)) { return -1; }
		if (!aTriggeringNodeInstance->ReadData(7, position)) { return -1; }
		if (!aTriggeringNodeInstance->ReadData(8, radius)) { return -1; }
		if (!aTriggeringNodeInstance->ReadData(11, targetPlayer)) { return -1; }
		if (!aTriggeringNodeInstance->ReadData(12, fall)) { return -1; }
		if (!targetPlayer && !aTriggeringNodeInstance->ReadData(9, targetPos)) { return -1; }
		if (!targetPlayer && !aTriggeringNodeInstance->ReadData(10, targetRadius)) { return -1; }

		myEnemyFactory = aTriggeringNodeInstance->ourPollingStation->GetEnemyFactory();
		myEnemyFactory->QueueSpawn(myIsActivePtr, charID, amount, intervalMin, intervalMax, position, radius, targetPos, targetRadius, targetPlayer, fall, true);
		myBoolPtrs[aTriggeringNodeInstance] = myIsActivePtr;
	}
	else if (aEnteredVia == 1)
	{
		if (myBoolPtrs.count(aTriggeringNodeInstance) > 0)
		{
			(*myBoolPtrs[aTriggeringNodeInstance]) = false;
		}
		//if (myIsActivePtr)
		//{
		//	(*myIsActivePtr) = false;
		//	myIsActivePtr = nullptr;
		//}
	}

	return 3;
}
