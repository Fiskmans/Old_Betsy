#include "pch.h"
#include "FoodCellar.h"
#include "Button.h"
#include "SpriteFactory.h"
#include "TextFactory.h"
#include "SpriteInstance.h"
#include "Scene.h"
#include "Sprite.h"
#include "Inventory.h"
#include <filesystem>
#include "TextInstance.h"
#include "AnimationComponent.h"
#include "TimeHandler.h"

std::unordered_map<ItemIdType, double> FoodCellar::myCalorieLookupMap;
std::unordered_map<ItemIdType, int> FoodCellar::myTypeAmounts;
double FoodCellar::ourStoredCalories = 0.0;

#define caloriesNeededForWinter 10000.0f 


void FoodCellar::InteractWith(Item* aHeldItem)
{
	PostMaster::GetInstance()->SendMessages(MessageType::LockPlayer);
	DisplayStorageMenu();
	/*if (aHeldItem)
	{
		if (CanStore(aHeldItem->myItemId) && aHeldItem->Consume())
		{
			Store(aHeldItem->myItemId);
		}
	}*/
}

void FoodCellar::Init(Entity* aEntity)
{
	myEntity = aEntity;


	myPlayerTradeItemsTopLeftPos = V2F(0.5f - (472.0f / Sprite::ourWindowSize.x), 0.5f - (203.0f / Sprite::ourWindowSize.y));
	myStorageTradeItemsTopLeftPos = V2F(0.5f + (120.0f / Sprite::ourWindowSize.x), 0.5f - (203.0f / Sprite::ourWindowSize.y));

	myTradeItemsXOffset = 88.0f / Sprite::ourWindowSize.x;
	myTradeItemsYOffset = 88.0f / Sprite::ourWindowSize.y;

	myPlayerTradeItemsColumns = 5;
	myTraderTradeItemsColumns = 5;
}

void FoodCellar::SetUp(Scene* aScene, TextFactory* aTextFactory, SpriteFactory* aSpriteFactory, Entity* aPlayerPtr)
{
	myScene = aScene;
	myTextFactory = aTextFactory;
	mySpriteFactory = aSpriteFactory;
	myPlayer = aPlayerPtr;

	myStorageBlackBackground = mySpriteFactory->CreateSprite("Data/UI/fadeScreen.dds");
	myStorageBlackBackground->SetColor(V4F(1.0f, 1.0f, 1.0f, 0.5f));
	myStorageBlackBackground->SetPosition(V2F(0.0f, 0.0f));
	myStorageBlackBackground->SetScale(V2F(1000.f, 1000.f));

	myStorageWindowBackground = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/StorageInventory.dds");
	myStorageWindowBackground->SetPivot(V2F(0.5f, 0.5f));
	myStorageWindowBackground->SetPosition(V2F(0.5f, 0.5f));

	myCaloriesBarXPosition = 0.5f - (332.0f / Sprite::ourWindowSize.x);
	myCaloriesBarXScaleForMaxFill = 150.0f;
	myStoredAmountSprite = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/StorageInventoryBar.dds");
	myStoredAmountSprite->SetPivot(V2F(0.0f, 0.5f));
	myStoredAmountSprite->SetPosition(V2F(myCaloriesBarXPosition, 0.5f - (457.0f / Sprite::ourWindowSize.y)));

	myStoredAmountChangeSprite = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/StorageInventoryBar.dds");
	myStoredAmountChangeSprite->SetPivot(V2F(0.0f, 0.5f));
	
	myStoredAmountChangeSprite->SetColor(V4F( 1.0f, 1.0f, 1.0f, 0.7f));

	myStoredAmountText = myTextFactory->CreateText();
	myStoredAmountText->SetPivot(V2F(0.5f, 0.5f));
	myStoredAmountText->SetPosition(V2F(0.5f, 0.25f));
	myStoredAmountText->SetText(std::to_string(ourStoredCalories) + " / " + std::to_string(caloriesNeededForWinter));
}

void FoodCellar::Update(const float aDeltaTime)
{
#if USEIMGUI
	WindowControl::Window("Food Cellar", []()
	{
		ImGui::Text("Calories Stored: %lf", ourStoredCalories);
	});
#endif // USEIMGUI

}

void FoodCellar::Reset()
{
	myScene = nullptr;
	myTextFactory = nullptr;
	mySpriteFactory = nullptr;
	myPlayer = nullptr;
	myEntity = nullptr;

	SAFE_DELETE(myStorageBlackBackground);
	SAFE_DELETE(myStorageWindowBackground);
	SAFE_DELETE(myStoredAmountSprite);
	SAFE_DELETE(myStoredAmountChangeSprite);
	SAFE_DELETE(myStoredAmountText);
}

void FoodCellar::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::InteractAnimationFinnished)
	{
		myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle, myIsOpen, false, false);
	}
}

bool FoodCellar::CanStore(ItemId aItem)
{
	return myCalorieLookupMap.count(aItem.ID) != 0;
}

void FoodCellar::Store(ItemId aItem)
{
	ourStoredCalories += myCalorieLookupMap[aItem.ID];
	if (myTypeAmounts.count(aItem.ID) == 0)
	{
		myTypeAmounts[aItem.ID] = 0;
	}
	myTypeAmounts[aItem.ID] += 1;
}

void FoodCellar::RemoveFromStorage(ItemId aItem)
{
	ourStoredCalories -= myCalorieLookupMap[aItem.ID];
	myTypeAmounts[aItem.ID] -= 1;
}

void FoodCellar::DisplayStorageMenu()
{
	TimeHandler::GetInstance().PauseTime();

	myPlayerInventory = myPlayer->GetComponent<Inventory>();

	myScene->AddSprite(myStorageBlackBackground);
	myScene->AddSprite(myStorageWindowBackground);
	myScene->AddSprite(myStoredAmountSprite);
	myScene->AddSprite(myStoredAmountChangeSprite);

	float storedPercentage = ourStoredCalories / caloriesNeededForWinter;
	float showedVal = CLAMP(0.0f, 1.11f, storedPercentage);
	myStoredAmountSprite->SetScale(V2F(showedVal * myCaloriesBarXScaleForMaxFill, 7.0f));

	myStoredAmountChangeSprite->SetPosition(V2F(myCaloriesBarXPosition + (storedPercentage * (600.0f / Sprite::ourWindowSize.x)), myStoredAmountSprite->GetPosition().y));
	myStoredAmountChangeSprite->SetScale(V2F(0.0f, 7.0f));

	auto vector = myPlayerInventory->FetchItems();
	
	for (auto& i : vector)
	{
		if (myCalorieLookupMap.count(i->myItemId.ID) != 0)
		{
			int amount = i->myAmount;
			for (int index = 0; index < amount; index++)
			{
				AddTradeItemToArea(i->myItemId, TradeArea::PlayerInventory, TradeArea::None);
			}
		}
	}

	for (auto& type : myTypeAmounts)
	{
		int amount = type.second;
		for (int index = 0; index < amount; index++)
		{
			AddTradeItemToArea(ItemFromID(type.first), TradeArea::TraderInventory, TradeArea::None);
		}
	}

	myCloseStorageButton = new Button();
	myCloseStorageButton->Init("Data/UI/UIButtons/Close_Normal.dds", "Data/UI/UIButtons/Close_Hover.dds", "Data/UI/UIButtons/Close_Pressed.dds", "Data/UI/UIButtons/Close_Disabled.dds", V2F(0.5f, 0.85f));
	myCloseStorageButton->SetScenePtr(myScene);
	myCloseStorageButton->SetOnPressedFunction([this] { CloseStorageMenu(); });
	myScene->AddSprite(myCloseStorageButton->GetCurrentSprite());

	myIsOpen = true;
	myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Interact, 0, false, false);

	myScene->AddText(myStoredAmountText);
	myStoredAmountText->SetText(std::to_string(CAST(int, ourStoredCalories)) + " / " + std::to_string(CAST(int, caloriesNeededForWinter)));
}

void FoodCellar::CloseStorageMenu()
{
	TimeHandler::GetInstance().ResumeTime();
	PostMaster::GetInstance()->SendMessages(MessageType::UnlockPlayer);

	ClearTradeAreas();
	myCloseStorageButton->RemoveAllSprites();
	SAFE_DELETE(myCloseStorageButton);
	myScene->RemoveSprite(myStorageBlackBackground);
	myScene->RemoveSprite(myStorageWindowBackground);
	myScene->RemoveSprite(myStoredAmountSprite);
	myScene->RemoveSprite(myStoredAmountChangeSprite);

	Message storedCaloriesMessage;
	storedCaloriesMessage.myMessageType = MessageType::StoredCalories;
	storedCaloriesMessage.myFloatValue = ourStoredCalories / caloriesNeededForWinter;
	PostMaster::GetInstance()->SendMessages(storedCaloriesMessage);

	myIsOpen = false;
	myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Interact, 1, false, false);

	myScene->RemoveText(myStoredAmountText);
}

bool FoodCellar::AddTradeItemToArea(const ItemId& aItemIDType, TradeArea aToTradeArea, TradeArea aFromTradeArea)
{
	bool newItem = true;
	std::vector<TradableRepresentation>* currentVectorPtr;
	V2F firstItemPos = V2F();
	int columnSize = 1;

	switch (aToTradeArea)
	{
	case TradeArea::PlayerInventory:
		currentVectorPtr = &myPlayerTradeItems;
		columnSize = myPlayerTradeItemsColumns;
		firstItemPos = myPlayerTradeItemsTopLeftPos;
		break;

	case TradeArea::TraderInventory:
		currentVectorPtr = &myStorageTradeItems;
		columnSize = myPlayerTradeItemsColumns;
		firstItemPos = myStorageTradeItemsTopLeftPos;
		break;
	default:
		SYSERROR("TRYING TO ADD TRADE ITEM TO NONEXISTANT UI AREA", "ERROR");
		currentVectorPtr = &myPlayerTradeItems;
		break;
	}

	int index = 0;
	for (auto& representation : *currentVectorPtr)
	{
		if (representation.myItemIDType == aItemIDType)
		{
			newItem = false;
			break;
		}
		else
		{
			index++;
		}
	}

	if (newItem)
	{
		//TODO FIX ITEMBUTTON NAME BASED ON ITEMIDTYPE


		std::string itemIconFullPath = Inventory::GetItemIconPath(aItemIDType);
		std::filesystem::path itemIconPath(itemIconFullPath);
		std::string itemIconSubString = itemIconFullPath.substr(0, itemIconFullPath.size() - itemIconPath.extension().string().size());
		std::string normalPath = itemIconFullPath;
		std::string pressedPath = itemIconFullPath;
		std::string hoverPath = itemIconSubString + "_hover.dds";
		std::string disabledPath = itemIconSubString + "_off.dds";
		V2F buttonPos = V2F();

		int xIndex = index % columnSize;
		int yIndex = index / columnSize;

		buttonPos = V2F(firstItemPos.x + myTradeItemsXOffset * xIndex, firstItemPos.y + myTradeItemsYOffset * yIndex);

		TradeArea buttonClickToArea;
		switch (aToTradeArea)
		{
		case TradeArea::PlayerInventory:
			buttonClickToArea = TradeArea::TraderInventory;
			break;
		case TradeArea::TraderInventory:
			buttonClickToArea = TradeArea::PlayerInventory;
			break;
		default:
			break;
		}

		TradableRepresentation tradeRep;
		tradeRep.myTradeArea = aToTradeArea;
		tradeRep.myItemIDType = aItemIDType;
		tradeRep.myButton = new Button();
		tradeRep.myButton->SetSpriteFactory(mySpriteFactory);
		tradeRep.myButton->SetScenePtr(myScene);
		tradeRep.myButton->Init(normalPath, hoverPath, pressedPath, disabledPath, buttonPos);
		tradeRep.myButton->SetOnPressedFunction([this, aItemIDType, buttonClickToArea, aToTradeArea] { AddTradeItemToArea(aItemIDType, buttonClickToArea, aToTradeArea); });
		tradeRep.myAmount = 1;
		tradeRep.myAmountText = myTextFactory->CreateText();
		tradeRep.myAmountText->SetText(std::to_string(tradeRep.myAmount));
		tradeRep.myAmountText->SetPosition(tradeRep.myButton->GetCurrentSprite()->GetPosition());
		currentVectorPtr->push_back(tradeRep);

		myScene->AddText(tradeRep.myAmountText);
		myScene->AddSprite(tradeRep.myButton->GetCurrentSprite());

	}
	else
	{
		if ((*currentVectorPtr)[index].myAmount == 0)
		{
			(*currentVectorPtr)[index].myButton->ActuallyEnable();
			(*currentVectorPtr)[index].myButton->SetToNormal();
		}
		(*currentVectorPtr)[index].myAmount++;
		(*currentVectorPtr)[index].myAmountText->SetText(std::to_string((*currentVectorPtr)[index].myAmount));
	}

	if (aFromTradeArea != TradeArea::None)
	{
		if (aFromTradeArea == TradeArea::PlayerInventory)
		{
			auto vector = myPlayerInventory->FetchItems();
			for (auto& item : vector)
			{
				if (item->myItemId == aItemIDType)
				{
					item->myAmount--;
					break;
				}

			}
			Store(aItemIDType);
			V2F scale = myStoredAmountChangeSprite->GetScale();
			scale.x += (myCalorieLookupMap[aItemIDType.ID] / caloriesNeededForWinter) * myCaloriesBarXScaleForMaxFill;
			myStoredAmountChangeSprite->SetScale(scale);
			if (scale.x >= 0)
			{
				myStoredAmountChangeSprite->SetColor(V4F(0.0f, 1.0f, 0.0f, 0.7f));
			}
		}
		else
		{
			bool newItemToInventory = true;
			auto vector = myPlayerInventory->FetchItems();
			for (auto& item : vector)
			{
				if (item->myItemId == aItemIDType)
				{
					newItemToInventory = false;
					item->myAmount++;
					break;
				}
			}

			if (newItemToInventory)
			{
				ItemCreationStruct itemstruct;
				itemstruct.myItemId = aItemIDType.ID;
				Message mess;
				mess.myMessageType = MessageType::SpawnItem;
				mess.myData = &itemstruct;
				PostMaster::GetInstance()->SendMessages(mess);
			}
			RemoveFromStorage(aItemIDType);
			V2F scale = myStoredAmountChangeSprite->GetScale();
			scale.x -= (myCalorieLookupMap[aItemIDType.ID] / caloriesNeededForWinter) * myCaloriesBarXScaleForMaxFill;
			myStoredAmountChangeSprite->SetScale(scale);
			if (scale.x < 0)
			{
				myStoredAmountChangeSprite->SetColor(V4F(1.0f, 0.0f, 0.0f, 0.7f));
			}
		}
		RemoveTradeItemFromArea(aItemIDType, aFromTradeArea);
	}
	return true;
}

bool FoodCellar::RemoveTradeItemFromArea(const ItemId& aItemIDType, TradeArea aTradeArea)
{
	std::vector<TradableRepresentation>* currentVectorPtr;

	switch (aTradeArea)
	{
	case TradeArea::PlayerInventory:
		currentVectorPtr = &myPlayerTradeItems;
		break;
	case TradeArea::TraderInventory:
		currentVectorPtr = &myStorageTradeItems;
		break;
	default:
		SYSERROR("TRYING TO REMOVE TRADEITEM FROM NONEXISTANT UI AREA", "ERROR");
		currentVectorPtr = &myPlayerTradeItems;
		break;
	}

	int index = 0;
	for (auto& representation : *currentVectorPtr)
	{
		if (representation.myItemIDType == aItemIDType)
		{
			representation.myAmount--;
			representation.myAmountText->SetText(std::to_string(representation.myAmount));
			if (representation.myAmount <= 0)
			{
				representation.myButton->ActuallyDisable();
			}
			return true;
			break;
		}
		index++;
	}
	return true;
}

void FoodCellar::ClearTradeAreas()
{
	for (auto& representation : myPlayerTradeItems)
	{
		representation.myButton->RemoveAllSprites();
		myScene->RemoveText(representation.myAmountText);
		SAFE_DELETE(representation.myButton);
		SAFE_DELETE(representation.myAmountText);
	}

	myPlayerTradeItems.clear();

	for (auto& representation : myStorageTradeItems)
	{
		representation.myButton->RemoveAllSprites();
		myScene->RemoveText(representation.myAmountText);
		SAFE_DELETE(representation.myButton);
		SAFE_DELETE(representation.myAmountText);
	}

	myStorageTradeItems.clear();
}

void FoodCellar::PopulateCalorieLookup()
{
	if (myCalorieLookupMap.empty())
	{
		FiskJSON::Object root;
		try
		{
			root.Parse(Tools::ReadWholeFile("Data/Metrics/Calories.json"));
			for (auto& plant : root)
			{
				float value;
				if (plant.second->GetIf(value))
				{
					myCalorieLookupMap[ItemIdFromString(plant.first).ID] = value;
				}
			}
		}
		catch (const FiskJSON::Invalid_JSON& e)
		{
			LOGERROR(e.what(), "Data/Metrics/Calories.json");
		}
		catch (const FiskJSON::Invalid_Object& e)
		{
			LOGERROR(e.what(), "Data/Metrics/Calories.json");
		}
	}
}

std::unordered_map<ItemIdType, int> FoodCellar::GetItemsAndAmounts()
{
	return myTypeAmounts;
}

double FoodCellar::GetStoredTotalCalories()
{
	return ourStoredCalories;
}

void FoodCellar::SetPtrs()
{
	Message requestPtrsMessage;
	requestPtrsMessage.myMessageType = MessageType::RequestUISetupPtrs;
	requestPtrsMessage.myData = this;
	PostMaster::GetInstance()->SendMessages(requestPtrsMessage);
}

void FoodCellar::SetPlayerInventory(Inventory* aInventory)
{
	myPlayerInventory = aInventory;
}
