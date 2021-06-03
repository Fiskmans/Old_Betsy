#include "pch.h"
#include "GrowthSpot.h"
#include "Growable.h"
#include "GBPhysXStaticComponent.h"
#include "DataStructs.h"
#include "Random.h"
#include "PlantLoader.h"
#include "AssetManager.h"
#include "Game\Mesh.h"
#include "Game\Audio.h"


const char* DryFilePath = "G_Soil_Dry/G_Soil_Dry.fbx";
const char* WetFilePath = "G_Soil_Wet/G_Soil_Wet.fbx";
const char* UnPlowedFilePath = "G_Soil_Unplowed/G_Soil_Unplowed.fbx";

std::unordered_map<ItemIdType, std::string> GrowthSpot::ourSeedMapping;


void GrowthSpot::PreInit(std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aPool)
{
	myEntityPool = aPool;
	myEntityVector = aEntityVector;
}

void GrowthSpot::SetGBPhysXPointer(GBPhysX* aGBPhysX)
{
	myGBPhysX = aGBPhysX;
}

void GrowthSpot::Spawn()
{
	myIsWatered = false;
	myIsTilled = false;

	Mesh* mesh = myEntity->GetComponent<Mesh>();
	if (mesh)
	{
		ONETIMEWARNING("Entity spawned with a GrowthSpot component and a mesh component, GrowthSpot needs to be able to control the mesh entirely", "");
	}
	else
	{
		Mesh* mesh = myEntity->AddComponent<Mesh>();
		mesh->Init(myEntity);

		mesh->SetUpModel(UnPlowedFilePath);
		mesh->AddModelToScene();
	}

	{
		GBPhysXStaticComponent* physx = myEntity->GetComponent<GBPhysXStaticComponent>();

		if (physx)
		{
			ONETIMEWARNING("Entity spawned with a GrowthSpot component and a physx component, GrowthSpot needs to be able to control the physx entirely", "");
		}
		else
		{
			physx = myEntity->AddComponent<GBPhysXStaticComponent>();
			physx->Init(myEntity);
			physx->SetGBPhysXPtr(myGBPhysX);
			physx->SetFilePath(UnPlowedFilePath);
		}
	}
}

void GrowthSpot::RemovePlant()
{
	myPlant = nullptr;

	myIsTilled = false;
	myIsWatered = false;

	myEntity->GetComponent<Mesh>()->Reset();
	myEntity->RemoveComponent<Mesh>();
	Mesh* mesh = myEntity->AddComponent<Mesh>();
	mesh->Init(myEntity);
	mesh->SetUpModel(UnPlowedFilePath);
	mesh->AddModelToScene();
}

void GrowthSpot::InteractWith(Item* aItem)
{
	if (!myIsTilled)
	{
		if (aItem && aItem->myItemId == PreCalculatedItemIds::Hoe)
		{
			QueuedEvent qEvent;
			qEvent.timer = 0.6f;
			qEvent.event = GrowthSpotEvent::HoeHitsGround;
			myQueuedEvents.push_back(qEvent);

			Message message;
			message.myMessageType = MessageType::HoeingGround;
			SendMessages(message);
		}
		else
		{
			Message message;
			message.myMessageType = MessageType::ShouldUseItem;
			message.myIntValue = PreCalculatedItemIds::Hoe.ID;
			SendMessages(message);
		}
	}
	if (myIsTilled)
	{
		if (myPlant)
		{
			if (aItem && aItem->myItemId == PreCalculatedItemIds::Basket && myPlant->GetComponent<Growable>()->GetPlantInstance()->GetIsHarvestable())
			{
				QueuedEvent qEvent;
				qEvent.timer = 2.1f;
				qEvent.event = GrowthSpotEvent::HarvestingPlant;
				qEvent.item = aItem;
				myQueuedEvents.push_back(qEvent);

				Message message;
				message.myMessageType = MessageType::HarvestingPlant;
				SendMessages(message);
			}
			else if (myPlant->GetComponent<Growable>()->GetPlantInstance()->GetIsHarvestable() && myPlant->GetComponent<Growable>()->GetPlantInstance()->GetStage() > 2)
			{
				Message message;
				message.myMessageType = MessageType::ShouldUseItem;
				message.myIntValue = PreCalculatedItemIds::Basket.ID;
				SendMessages(message);
			}
			else if (!GetIsMOIST() && aItem && aItem->myItemId == PreCalculatedItemIds::WateringCanFilled)
			{
				if (aItem->UseCharge())
				{
					QueuedEvent qEvent;
					qEvent.timer = 0.6f;
					qEvent.event = GrowthSpotEvent::GroundGetsWet;
					myQueuedEvents.push_back(qEvent);

					myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::Water);

					Message message;
					message.myMessageType = MessageType::WateringPlant;
					SendMessages(message);
				}
				else
				{
					Message message;
					message.myMessageType = MessageType::SendUIGameMessage;
					message.myText = "Waterbucket is empty";
					SendMessages(message);
				}
			}
			else if (!GetIsMOIST() && aItem && aItem->myItemId.ID == PreCalculatedItemIds::WateringCan.ID)
			{
				Message message;
				message.myMessageType = MessageType::SendUIGameMessage;
				message.myText = "Waterbucket is empty";
				SendMessages(message);
			}
			else if (!GetIsMOIST())
			{
				
				Message message1;
				message1.myMessageType = MessageType::ShouldUseItem;
				message1.myIntValue = PreCalculatedItemIds::WateringCan.ID;
				SendMessages(message1);

				Message message2;
				message2.myMessageType = MessageType::ShouldUseItem;
				message2.myIntValue = PreCalculatedItemIds::WateringCanFilled.ID;
				SendMessages(message2);
			}
		}
		else
		{
			if (aItem && ourSeedMapping.count(aItem->myItemId.ID) != 0)
			{
				if (aItem->Consume())
				{
					QueuedEvent qEvent;
					qEvent.timer = 2.f;
					qEvent.event = GrowthSpotEvent::PlantIsPlaces;
					qEvent.plantID = aItem->myItemId.ID;
					myQueuedEvents.push_back(qEvent);

					Message message;
					message.myMessageType = MessageType::PlantingSeed;
					SendMessages(message);
				}
			}
			else
			{
				Message message;
				message.myMessageType = MessageType::ShouldUseItem;
				message.myIntValue = PreCalculatedItemIds::Seeds.ID;
				SendMessages(message);

				Message seedsMessage;
				seedsMessage.myMessageType = MessageType::SendUIGameMessage;
				seedsMessage.myText = "I need something to plant";
				SendMessages(seedsMessage);
			}
		}
	}
}

void GrowthSpot::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myPlant = nullptr;
}

void GrowthSpot::Update(const float aDeltaTime)
{
	for (int i = myQueuedEvents.size() - 1; i >= 0; --i)
	{
		if (myQueuedEvents[i].timer <= 0)
		{
			if (myQueuedEvents[i].event == GrowthSpotEvent::HoeHitsGround)
			{
				Plow();
			}
			else if (myQueuedEvents[i].event == GrowthSpotEvent::PlantIsPlaces)
			{
				SpawnPlant(ourSeedMapping[myQueuedEvents[i].plantID]);
			}
			else if (myQueuedEvents[i].event == GrowthSpotEvent::GroundGetsWet)
			{
				Water();
			}
			else if (myQueuedEvents[i].event == GrowthSpotEvent::HarvestingPlant)
			{
				myPlant->InteractWith(myQueuedEvents[i].item);
				myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::HarvestPlant);
				myQueuedEvents[i].item = nullptr;
			}

			myQueuedEvents.erase(myQueuedEvents.begin() + i);
		}
		else
		{
			myQueuedEvents[i].timer -= aDeltaTime;
		}
	}
}

void GrowthSpot::Reset()
{
	myPlant = nullptr;
}

void GrowthSpot::OnAttach()
{
}

void GrowthSpot::OnDetach()
{
}

void GrowthSpot::OnKillMe()
{
}

void GrowthSpot::SubscribeToMyMessages()
{
	PostMaster::GetInstance()->Subscribe(MessageType::NewDay, this);
}

void GrowthSpot::UnSubscribeToMyMessages()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::NewDay, this);
}

bool GrowthSpot::Water()
{
	if (myIsTilled)
	{
		if (!myIsWatered)
		{
			myEntity->GetComponent<Mesh>()->Reset();
			myEntity->RemoveComponent<Mesh>();
			Mesh* mesh = myEntity->AddComponent<Mesh>();
			mesh->Init(myEntity);
			mesh->SetUpModel(WetFilePath);
			mesh->UpdateInstancePosition();
			mesh->AddModelToScene();
			myIsWatered = true;
			return true;
		}
	}
	return false;
}

void GrowthSpot::Dry()
{
	if (myIsTilled)
	{
		if (myIsWatered)
		{
			myEntity->GetComponent<Mesh>()->Reset();
			myEntity->RemoveComponent<Mesh>();
			Mesh* mesh = myEntity->AddComponent<Mesh>();
			mesh->Init(myEntity);
			mesh->SetUpModel(DryFilePath);
			mesh->UpdateInstancePosition();
			mesh->AddModelToScene();
		}
		myIsWatered = false;
	}
}

void GrowthSpot::Plow()
{
	if (!myIsTilled)
	{
		myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::Plow);

		myIsTilled = true;
		myEntity->GetComponent<Mesh>()->Reset();
		myEntity->RemoveComponent<Mesh>();
		Mesh* mesh = myEntity->AddComponent<Mesh>();
		mesh->Init(myEntity);
		mesh->SetUpModel(DryFilePath);
		mesh->UpdateInstancePosition();
		mesh->AddModelToScene();

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
			buffer.aFilePath = GetRandomDirt();
			spawnDirtBall.myData = &buffer;
			PostMaster::GetInstance()->SendMessages(spawnDirtBall);
		}
	}
}

bool GrowthSpot::GetIsMOIST()
{
	return myIsWatered;
}

bool GrowthSpot::GetIsHoed()
{
	return myIsTilled;
}

void GrowthSpot::PopulateSeeds()
{
	if (ourSeedMapping.empty())
	{
		FiskJSON::Object& root = AssetManager::GetInstance().GetJSON("metrics/plantables.json").GetAsJSON();

		int size = 1; 
		root["StackSize"].GetIf(size);

		for (auto& plant : root)
		{
			std::string value;
			if (plant.second->GetIf(value))
			{
				ourSeedMapping[RegisterItem(plant.first, size).ID] = value;
			}
		}
	}

}

bool GrowthSpot::CanBePlanted(int aItemID)
{
	return ourSeedMapping.count(aItemID) != 0;
}

const std::unordered_map<ItemIdType, std::string>& GrowthSpot::GetSeedMap()
{
	return ourSeedMapping;
}

void GrowthSpot::SpawnPlant(const std::string& aPlantPath)
{
	myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::Plant);

	Entity* ent = myEntityPool->Retrieve();
	ent->SetEntityType(EntityType::EnvironmentInteractable);
	ent->AddComponent<Growable>()->Init(ent);
	ent->GetComponent<Growable>()->SetPlant(aPlantPath, myEntity);
	ent->Spawn(myEntity->GetPosition());
	LOGVERBOSE("Spawned a plant");
	myEntityVector->push_back(ent);
	myPlant = ent;
}

void GrowthSpot::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::NewDay:
	{
		if (!myPlant)
		{
			Dry();
		}
	}
	break;
	default:
		break;
	}
}

std::string GrowthSpot::GetRandomDirt()
{
	int rand = Tools::RandomRange(1, 3);
	std::string buffer;
	switch (rand)
	{
	case 1:
		buffer = "G_dirt_01/G_dirt_01.fbx";
		break;
	case 2:
		buffer = "G_dirt_02/G_dirt_02.fbx";
		break;
	case 3:
		buffer = "G_dirt_03/G_dirt_03.fbx";
		break;
	default:
		buffer = "G_dirt_01/G_dirt_01.fbx";
		break;
	}
	return buffer;
}
