#include "pch.h"
#include "NodeSpawnEnemy.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "..//Game/DataStructs.h"
#include "..//Game/EnemyFactory.h"

NodeSpawnEnemy::NodeSpawnEnemy() :
	myEnemyFactory(nullptr)
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("CharacterID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Position", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Rotation", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("CreatedEnemy", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));
	myPins.push_back(CPin("SpawnFalling", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	
	SetPinType<int>(2);
	SetPinType<V3F>(3);
	SetPinType<V3F>(4);
	SetPinType<GameObjectId>(5);
	SetPinType<bool>(6);
}

int NodeSpawnEnemy::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int charID;
	V3F position;
	V3F rotation;
	if (!aTriggeringNodeInstance->ReadData(2, charID)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(3, position)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(4, rotation)) { return -1; }
	
	myEnemyFactory = aTriggeringNodeInstance->ourPollingStation->GetEnemyFactory();
	Entity* enemy = myEnemyFactory->CreateEnemy("", charID, position, rotation, V3F(1.0f, 1.0f, 1.0f), -1, -1, true);
	if (enemy)
	{
		aTriggeringNodeInstance->WriteData(5, GameObjectId(enemy));
	}

	bool fall;
	if (aTriggeringNodeInstance->ReadData(6, fall) && fall)
	{
		enemy->SendEntityMessage(EntityMessage::StartFalling);
	}

	return 1;
}
