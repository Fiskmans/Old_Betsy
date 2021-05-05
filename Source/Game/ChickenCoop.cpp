#include "pch.h"
#include "ChickenCoop.h"

void ChickenCoop::InteractWith(Item* aHeldItem)
{
	if (aHeldItem)
	{

		if (aHeldItem->myItemId == PreCalculatedItemIds::Chicken)
		{
			if (aHeldItem->Consume())
			{
				myChickenAmount++;
				Message mess;
				mess.myMessageType = MessageType::SpawnChicken;
				mess.myData = myEntity;
				PostMaster::GetInstance()->SendMessages(mess);

				Message message;
				message.myMessageType = MessageType::LeavingHen;
				SendMessages(message);

			}
		}
		else if (aHeldItem->myItemId == PreCalculatedItemIds::Basket)
		{
			if (myEggAmount > 0)
			{
				ItemCreationStruct struc;
				struc.myItemId = PreCalculatedItemIds::Eggs.ID;
				struc.myPosition = myEntity->GetPosition();
				struc.myAmount = myEggAmount;
				Message mess;
				mess.myData = &struc;
				mess.myMessageType = MessageType::SpawnItem;
				PostMaster::GetInstance()->SendMessages(mess);

				Message message;
				message.myMessageType = MessageType::HarvestingPlant;
				SendMessages(message);

				myEggAmount = 0;
			}
		}
	}
}

void ChickenCoop::Init(Entity* aEntity)
{
	myEntity = aEntity;
	PostMaster::GetInstance()->Subscribe(MessageType::NewDay, this);
}

void ChickenCoop::Update(const float aDeltaTime)
{
}

void ChickenCoop::Reset()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::NewDay, this);
}

bool ChickenCoop::HasEggs()
{
	return myEggAmount > 0;
}

void ChickenCoop::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::NewDay:
	{
		myEggAmount = myChickenAmount;
	}
	break;
	}
}
