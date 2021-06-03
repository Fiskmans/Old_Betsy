#include "pch.h"
#include "WildPlant.h"
#include "GrowthSpot.h"
#include "Random.h"
#include "PlantLoader.h"
#include "AnimationComponent.h"
#include "DataStructs.h"
#include "Game\Audio.h"

WildPlant::WildPlant()
{
}

WildPlant::~WildPlant()
{
}

void WildPlant::Init(Entity* aEntity)
{
	mySeedMap = GrowthSpot::GetSeedMap();
	for (auto& index : mySeedMap)
	{
		mySeedIds.push_back(index.first);
	}
	int size = mySeedIds.size();
	int randomIndex = Tools::RandomRange(0, size - 1);
	SetPlantTypeID(mySeedIds[randomIndex]);

	auto plantInstance = PlantLoader::GetInstance().GetPlant(mySeedMap[myPlantTypeID]);
	myPlantModelName = ""; // plantInstance->myBasePlant->myStages.back().myModelPath;

	SAFE_DELETE(plantInstance);
}

void WildPlant::Update(const float aDeltaTime)
{
	if (myStartTimer)
	{
		myPickPlantTimer -= aDeltaTime;

		if (myPickPlantTimer <= 0)
		{
			myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::HarvestPlant);

			myEntity->SetIsAlive(false);
			Message giveItemToPlayerMessage;
			giveItemToPlayerMessage.myMessageType = MessageType::SpawnItem;

			myItemData.myPosition = myEntity->GetPosition();
			myItemData.myItemId = myPlantTypeID;
			myItemData.myAmount = 1;

			giveItemToPlayerMessage.myData = &myItemData;

			PostMaster::GetInstance()->SendMessages(giveItemToPlayerMessage);

			//SPAWN DIRT

			for (int index = 0; index < 8; index++)
			{
				Message spawnDirtBall;
				spawnDirtBall.myMessageType = MessageType::SpawnDynamicObject;

				DynamicObjectInstanceBuffer buffer;

				buffer.position[0] = myEntity->GetPosition().x;
				buffer.position[1] = myEntity->GetPosition().y;
				buffer.position[2] = myEntity->GetPosition().z;
				buffer.scale[0] = 1.0f;
				buffer.scale[1] = 1.0f;
				buffer.scale[2] = 1.0f;

				int rand = Tools::RandomRange(1, 3);
				switch (rand)
				{
				case 1:
					buffer.aFilePath = "G_dirt_01/G_dirt_01.fbx";
					break;
				case 2:
					buffer.aFilePath = "G_dirt_02/G_dirt_02.fbx";
					break;
				case 3:
					buffer.aFilePath = "G_dirt_03/G_dirt_03.fbx";
					break;
				default:
					buffer.aFilePath = "G_dirt_01/G_dirt_01.fbx";
					break;
				}

				spawnDirtBall.myData = &buffer;
				PostMaster::GetInstance()->SendMessages(spawnDirtBall);
			}
		}
	}
}

void WildPlant::Reset()
{
	myStartTimer = false;
	myPickPlantTimer = 2.1f;
	mySeedMap.clear();
	mySeedIds.clear();
}

void WildPlant::InteractWith(Item* aHeldItem)
{
	if (aHeldItem && aHeldItem->myItemId == PreCalculatedItemIds::Basket)
	{
		myStartTimer = true;
		myPickPlantTimer = 2.1f;

		PostMaster::GetInstance()->SendMessages(MessageType::HarvestingPlant);
	}
	else
	{
		Message message;
		message.myMessageType = MessageType::ShouldUseItem;
		message.myIntValue = PreCalculatedItemIds::Basket.ID;
		SendMessages(message);
	}
}

void WildPlant::SetPlantTypeID(int aTypeID)
{
	myPlantTypeID = aTypeID;
}

int WildPlant::GetPlantTypeID()
{
	return myPlantTypeID;
}

void WildPlant::SetPlayerPtr(Entity* aPlayerPtr)
{
	myPlayerPtr = aPlayerPtr;
}

std::string WildPlant::GetModelName()
{
	return myPlantModelName;
}
