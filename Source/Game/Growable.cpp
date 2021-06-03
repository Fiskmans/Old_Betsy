#include "pch.h"
#include "Growable.h"
#include "PlantLoader.h"
#include "GrowthSpot.h"
#include "GBPhysXStaticComponent.h"
#include <Random.h>
#include "Game\Mesh.h"

Growable::~Growable()
{
	UnSubscribeToMyMessages();
	SAFE_DELETE(myPlant);
}

void Growable::SetPlant(const std::string& aPlantFile, Entity* aGroundEntity)
{
	myPlant = PlantLoader::GetInstance().GetPlant(aPlantFile);
	myGroundEntity = aGroundEntity;
}


void Growable::Spawn()
{
	if (!myPlant || !myPlant->myBasePlant)
	{
		SYSWARNING("Growable Component tried to spawn without a plant (failed to load or didn't try) removing self", "");
		myEntity->RemoveComponent<Growable>();
		return;
	}

	Mesh* mesh = myEntity->GetComponent<Mesh>();
	if (mesh)
	{
		ONETIMEWARNING("Entity spawned with a Growable component and a mesh component, growable needs to be able to control the mesh entirely", myPlant->myBasePlant->myFilePath);
		return;
	}
	{
		Mesh* mesh = myEntity->AddComponent<Mesh>();
		mesh->Init(myEntity);

		mesh->SetUpModel(myPlant->GetCurrentStage().myModelPath);
		mesh->UpdateInstancePosition();
		mesh->AddModelToScene();
	}
}

void Growable::InteractWith(Item* aHeldItem)
{
	if (aHeldItem)
	{
		if (aHeldItem->myItemId == PreCalculatedItemIds::WateringCan)
		{
			if (aHeldItem->UseCharge())
			{
				if (Water())
				{
					return;
				}
			}
		}
	}
	if (myPlant->GetIsHarvestable())
	{
		Harvest();
	}
}

bool Growable::Water()
{
	GrowthSpot* spot = myGroundEntity->GetComponent<GrowthSpot>();
	if (spot)
	{
		return spot->Water();
	}
	return false;
}

void Growable::Harvest()
{
	for (auto& yield : myPlant->GetCurrentStage().myYields)
	{
		float amount = yield.myAmountAndChance; // Copy to local
		ItemCreationStruct package;
		package.myItemId = yield.myItemType.ID;
		package.myPosition = myEntity->GetPosition();
		package.myAmount = amount;
		if (Tools::RandomNormalized() < (amount - std::floor(amount)))
		{
			package.myAmount += 1;
		}

		Message createMessage;
		createMessage.myMessageType = MessageType::SpawnItem;
		createMessage.myData = &package;
		PostMaster::GetInstance()->SendMessages(createMessage);
	}


	myEntity->Kill();
	GrowthSpot* spot = myGroundEntity->GetComponent<GrowthSpot>();
	if (spot)
	{
		spot->RemovePlant();
	}
}

PlantInstance* Growable::GetPlantInstance()
{
	return myPlant;
}


void Growable::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myEntity->SetRotation(V3F(0, Tools::RandomNormalized() * PI, 0));
	SubscribeToMyMessages();
}

void Growable::Update(const float aDeltaTime)
{
}

void Growable::Reset()
{
	GrowthSpot* spot = myGroundEntity->GetComponent<GrowthSpot>();
	if (spot)
	{
		spot->RemovePlant();
	}
	UnSubscribeToMyMessages();
	SAFE_DELETE(myPlant);
}

void Growable::OnAttach()
{
}

void Growable::OnDetach()
{
}

void Growable::OnKillMe()
{
}

void Growable::SubscribeToMyMessages()
{
	PostMaster::GetInstance()->Subscribe(MessageType::NewDay, this);
}

void Growable::UnSubscribeToMyMessages()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::NewDay, this);
}

void Growable::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::NewDay:
	{
		GrowthSpot* spot = myGroundEntity->GetComponent<GrowthSpot>();
		if (spot)
		{
			if (spot->GetIsMOIST())
			{
				spot->Dry();
				myPlant->ProgressStage();

				myEntity->GetComponent<Mesh>()->Reset();
				myEntity->RemoveComponent<Mesh>();
				Mesh* mesh = myEntity->AddComponent<Mesh>();
				mesh->Init(myEntity);
				mesh->SetUpModel(myPlant->GetCurrentStage().myModelPath);
				mesh->AddModelToScene();
			}
		}
		else
		{
			LOGERROR("Plant is not planted in a growthspot", "");
		}
	}
	break;
	default:
		break;
	}
}
