#include "pch.h"
#include "WaterSource.h"

void WaterSource::InteractWith(Item* aHeldItem)
{
	if (aHeldItem && (aHeldItem->myItemId == PreCalculatedItemIds::WateringCan || (aHeldItem->myItemId == PreCalculatedItemIds::WateringCanFilled && aHeldItem->myCharges < myWaterCanSize)))
	{
		//aHeldItem->myCharges = myWaterCanSize;

		Message message;
		message.myMessageType = MessageType::FillingWaterbucket;
		SendMessages(message);
	}
	else if (aHeldItem && aHeldItem->myCharges >= myWaterCanSize)
	{
		//Full
		Message message;
		message.myMessageType = MessageType::SendUIGameMessage;
		message.myText = "Waterbucket is full";
		SendMessages(message);
	}
	else if (aHeldItem && (aHeldItem->myItemId != PreCalculatedItemIds::WateringCan || aHeldItem->myItemId != PreCalculatedItemIds::WateringCanFilled))
	{
		//Wrong item:

		Message message;
		message.myMessageType = MessageType::ShouldUseItem;
		message.myIntValue = PreCalculatedItemIds::WateringCanFilled.ID;
		SendMessages(message);
	}
}

void WaterSource::Init(Entity* aEntity)
{
}

void WaterSource::Update(const float aDeltaTime)
{
}

void WaterSource::Reset()
{
}
