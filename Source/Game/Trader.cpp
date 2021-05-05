#include "pch.h"
#include "Trader.h"
#include <Random.h>
#include "Inventory.h"
#include "SpriteFactory.h"
#include "TextFactory.h"
#include "Scene.h"
#include "SpriteInstance.h"
#include "TextInstance.h"
#include "Button.h"
#include <filesystem>
#include "Sprite.h"
#include "TraderAI.h"
#include "AIPollingStation.h"
#include "PlayerController.h"

void Trader::InteractWith(Item* aHeldItem)
{
	StartTrading();
}

void Trader::Init(Entity* aEntity)
{
	myEntity = aEntity;

	FiskJSON::Object root;
	try
	{
		root.Parse(Tools::ReadWholeFile("Data/Metrics/Trader.json"));
	}
	catch (const std::exception& e)
	{
		SYSERROR(e.what(), "Data/Metrics/Trader.json");
		return;
	}
	for (auto& item : root["TradeValues"])
	{
		float value;
		if (item.second->GetIf(value))
		{
			myTradePowerMap[ItemIdFromString(item.first)] = value;
		}
	}


	for (auto& lootTable : root["LootTables"].Get<FiskJSON::Array>())
	{
		LootTable table;
		for (auto& item : (*lootTable)["Items"].Get<FiskJSON::Array>())
		{
			std::string id;
			if (item->GetIf(id))
			{
				table.items.push_back(ItemIdFromString(id));
			}
		}

		auto& validity = (*lootTable)["ValidDays"];
		std::string string;
		table.validDays.reset();

		if (validity.GetIf(string))
		{
			if (string == "All" || string == "all")
			{
				table.validDays.set();
			}
		}
		else
		{
			if (validity.Has("From") && validity.Has("To"))
			{
				int from;
				int to;
				if (validity["From"].GetIf(from) && validity["To"].GetIf(to))
				{
					from--;
					to--;
					if (from > 0 && from < AMOUNTOFDAYS && to >= from)
					{
						for (size_t i = from; i < to && i < AMOUNTOFDAYS; i++)
						{
							table.validDays.set(i);
						}
					}
				}
			}
			else
			{
				for (auto& day : validity.Get<FiskJSON::Array>())
				{
					int dayIndex;
					if (day->GetIf(dayIndex))
					{
						dayIndex--;
						if (dayIndex > 0 && dayIndex < AMOUNTOFDAYS)
						{
							table.validDays.set(dayIndex);
						}
					}
				}
			}
		}
		myLootTables.push_back(table);
	}

	float valueLeft;
	if (root["TraderValue"].GetIf(valueLeft))
	{
		GenerateInventory(valueLeft, 1); //TODO SET DAY
	}

	myPlayerTradeItemsTopLeftPos = V2F(0.5f - (659.0f / Sprite::ourWindowSize.x), 0.5f - (203.0f / Sprite::ourWindowSize.y));
	myPlayerTradeItemsOfferedTopLeftPos = V2F(0.5f - (100.0f / Sprite::ourWindowSize.x), 0.5f - (308.0f / Sprite::ourWindowSize.y));
	myTraderTradeItemsOfferedTopLeftPos = V2F(0.5f + (100.0f / Sprite::ourWindowSize.x), 0.5f - (308.0f / Sprite::ourWindowSize.y));
	myTraderTradeItemsTopLeftPos = V2F(0.5f + (307.0f / Sprite::ourWindowSize.x), 0.5f - (203.0f / Sprite::ourWindowSize.y));


	myTradeItemsXOffset = 88.0f / Sprite::ourWindowSize.x;
	myTradeItemsYOffset = 88.0f / Sprite::ourWindowSize.y;

	myPlayerTradeItemsColumns = 5;
	myPlayerTradeItemsOfferedColumns = 1;
	myTraderTradeItemsColumns = 5;
	myTraderTradeItemsOfferedColumns = 1;

}

void Trader::Update(const float aDeltaTime)
{
	if (myIsTrading)
	{
#if USEIMGUI
		if (ImGui::Begin("Trader inventory"))
		{

			if (ImGui::Button("Trade"))
			{
				DoTrade();
			}
			float trade = EvaluateTrade();
			ImGui::Text("Trade value: " PFFLOAT, trade);
			trade += 50.0f;
			trade = CLAMP(0.f, 100.f, trade);
			trade /= 100.f;
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(LERP(1, 0, trade), LERP(0, 1, trade), 0, 1));
			ImGui::ProgressBar(trade);
			ImGui::PopStyleColor(1);
			trade -= 0.5f;


			ImGui::Separator();
			ImGui::Columns(2);
			ImGui::PushID("offer");
			ImGui::PushID("player");
			ImGui::Text("Offered: ");
			for (size_t i = 0; i < myPlayerItems.size(); i++)
			{
				if (myPlayerItems[i].myOffered > 0)
				{
					if (ImGui::Selectable((StringFromItemId(myPlayerItems[i].myItemId) + ": " + std::to_string(myPlayerItems[i].myOffered)).c_str()))
					{
						myPlayerItems[i].myOffered--;
					}
				}
			}
			ImGui::PopID();
			ImGui::NextColumn();
			ImGui::PushID("trader");
			ImGui::Text("Offered: ");
			for (size_t i = 0; i < myItems.size(); i++)
			{
				if (myItems[i].myOffered > 0)
				{
					if (ImGui::Selectable((StringFromItemId(myItems[i].myItemId) + ": " + std::to_string(myItems[i].myOffered)).c_str()))
					{
						myItems[i].myOffered--;
					}
				}
			}
			ImGui::PopID();
			ImGui::NextColumn();
			ImGui::PopID();
			ImGui::Separator();
			ImGui::Columns(2);
			ImGui::PushID("has");
			ImGui::PushID("player");
			ImGui::Text("Has: ");
			for (size_t i = 0; i < myPlayerItems.size(); i++)
			{
				if (ImGui::Selectable((StringFromItemId(myPlayerItems[i].myItemId) + ": " + std::to_string(myPlayerItems[i].myAmount - myPlayerItems[i].myOffered)).c_str()))
				{
					OfferFromPlayer(i);
				}
			}
			ImGui::PopID();
			ImGui::NextColumn();
			ImGui::PushID("trader");
			ImGui::Text("Has: ");
			for (size_t i = 0; i < myItems.size(); i++)
			{
				if (ImGui::Selectable((StringFromItemId(myItems[i].myItemId) + ": " + std::to_string(myItems[i].myAmount - myItems[i].myOffered)).c_str()))
				{
					Offer(i);
				}
			}
			ImGui::PopID();
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::End();
#endif
	}
}

void Trader::Reset()
{
}

void Trader::SetPlayerInventory(Inventory* aInventory)
{
	myPlayerInventory = aInventory;
}

void Trader::PrepareUI(Scene* aScenePtr, SpriteFactory* aSpriteFactoryPtr, TextFactory* aTextFactoryPtr)
{
	myScene = aScenePtr;
	mySpriteFactory = aSpriteFactoryPtr;
	myTextFactory = aTextFactoryPtr;

	myTradeBlackBackground = mySpriteFactory->CreateSprite("Data/UI/fadeScreen.dds");
	myTradeBlackBackground->SetPivot(V2F(0.0f, 0.0f));
	myTradeBlackBackground->SetPosition(V2F(0.0f, 0.0f));
	myTradeBlackBackground->SetScale(V2F(1000.0f, 1000.0f));
	myTradeBlackBackground->SetColor(V4F(1.0f, 1.0f, 1.0f, 0.7f));

	myTradeWindowBackground = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/tradingInventory.dds");
	myTradeWindowBackground->SetPivot(V2F(0.5f, 0.5f));
	myTradeWindowBackground->SetPosition(V2F(0.5f, 0.5f));

	myFairTradeSprite = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/Fairtrade_icon.dds");
	myFairTradeSprite->SetPivot(V2F(0.5f, 0.5f));
	myFairTradeSprite->SetPosition(V2F(0.5f, 0.5f + 370.0f / Sprite::ourWindowSize.y));

}

namespace noSymBolColidePlx
{
	template<typename S>
	auto select_random(const S& s, size_t n)
	{
		auto it = std::begin(s);
		// 'advance' the iterator n times
		std::advance(it, n);
		return it;
	}
}

void Trader::StartTrading()
{
	if (myPlayerInventory)
	{
		myIsTrading = true;
		FetchTradables();
		DisplayTradeWindow();
	}
#ifdef _DEBUG
	else
	{
		SYSWARNING("Trader component needs access to the players inventory before you can trade with it", "Trader");
	}
#endif // _DEBUG
}

void Trader::StopTrading()
{
	myIsTrading = false;
	CloseTradeWindow();
	myEntity->GetComponent<TraderAI>()->StopTrading();
}

void Trader::DisplayTradeWindow()
{
	PostMaster::GetInstance()->SendMessages(MessageType::TradeStarted);

	myScene->AddSprite(myTradeBlackBackground);
	myScene->AddSprite(myTradeWindowBackground);
	myScene->AddSprite(myFairTradeSprite);

	myAcceptTradeButton = new Button();
	myAcceptTradeButton->Init("Data/UI/UIButtons/Accept_Normal.dds", "Data/UI/UIButtons/Accept_Hover.dds", "Data/UI/UIButtons/Accept_Pressed.dds", "Data/UI/UIButtons/Accept_Disabled.dds", V2F(0.45f, 0.96f));
	myAcceptTradeButton->SetScenePtr(myScene);

	myScene->AddSprite(myAcceptTradeButton->GetCurrentSprite());
	myAcceptTradeButton->SetOnPressedFunction([this] { DoTrade(); });

	myCancelTradeButton = new Button();
	myCancelTradeButton->Init("Data/UI/UIButtons/Close_Normal.dds", "Data/UI/UIButtons/Close_Hover.dds", "Data/UI/UIButtons/Close_Pressed.dds", "Data/UI/UIButtons/Close_Disabled.dds", V2F(0.55f, 0.96f));
	myCancelTradeButton->SetScenePtr(myScene);
	myScene->AddSprite(myCancelTradeButton->GetCurrentSprite());
	myCancelTradeButton->SetOnPressedFunction([this] { StopTrading(); });

	for (auto& playerItem : myPlayerItems)
	{
		for (int i = 0; i < playerItem.myAmount; i++)
		{
			AddTradeItemToArea(playerItem.myItemId, TradeArea::PlayerInventory, TradeArea::None);
		}
	}
	for (auto& traderItem : myItems)
	{
		for (int i = 0; i < traderItem.myAmount; i++)
		{
			AddTradeItemToArea(traderItem.myItemId, TradeArea::TraderInventory, TradeArea::None);
		}
	}
}

void Trader::CloseTradeWindow()
{
	PostMaster::GetInstance()->SendMessages(MessageType::TradeEnded);

	ClearTradeAreas();
	myScene->RemoveSprite(myTradeBlackBackground);
	myScene->RemoveSprite(myTradeWindowBackground);
	myScene->RemoveSprite(myFairTradeSprite);

	myAcceptTradeButton->RemoveAllSprites();
	myCancelTradeButton->RemoveAllSprites();

	/*myScene->RemoveSprite(myAcceptTradeButton->GetCurrentSprite());
	myScene->RemoveSprite(myCancelTradeButton->GetCurrentSprite());*/

	SAFE_DELETE(myAcceptTradeButton);
	SAFE_DELETE(myCancelTradeButton);
}

void Trader::OnAbleToTrade()
{
	myIsAbleToTrade = true;
	//change button icons or other feedback here
	myAcceptTradeButton->ActuallyEnable();

}

void Trader::OnInableToTrade()
{
	myIsAbleToTrade = false;
	//change button icons or other feedback here
	myAcceptTradeButton->ActuallyDisable();
}

void Trader::DoTrade()
{
	float value = EvaluateTrade();
	if (value > 0)
	{
		std::vector<Item*> playerItems = myPlayerInventory->FetchItems();
		// O(n^3) but meh how bad can it get. n shouldn't be much larger than 20 anyways
		for (auto& trade : myPlayerItems)
		{
			for (size_t i = 0; i < trade.myOffered; i++)
			{
				bool removed = false;
				for (size_t i = 0; i < playerItems.size(); i++)
				{
					if (playerItems[i]->myItemId == trade.myItemId && playerItems[i]->myAmount > 0)
					{
						playerItems[i]->myAmount--;
						removed = true;
						break;
					}
				}
				if (!removed)
				{
					return;
				}
			}
		}


		for (size_t i = 0; i < myItems.size(); i++)
		{
			if (myItems[i].myOffered > 0)
			{
				ItemCreationStruct itemstruct;
				itemstruct.myItemId = myItems[i].myItemId.ID;
				itemstruct.myAmount = myItems[i].myOffered;
				Message mess;
				mess.myMessageType = MessageType::SpawnItem;
				mess.myData = &itemstruct;
				PostMaster::GetInstance()->SendMessages(mess);

				myItems[i].myAmount -= myItems[i].myOffered;

				myItems[i].myOffered = 0;
			}
		}
		for (int i = int(myItems.size()) - 1; i >= 0; i--)
		{
			if (myItems[i].myAmount <= 0)
			{
				myItems.erase(myItems.begin() + i);
			}
		}

		FetchTradables();
		StopTrading();
	}
#ifdef _DEBUG
	else
	{
		SYSINFO("Trader value is not high enough [" + std::to_string(value) + "] needs to be positive");
	}
#endif // _DEBUG
}

void Trader::GenerateInventory(float aTradeValue, char aDay)
{
	std::set<ItemId> validItems;
	for (auto& i : myLootTables)
	{
		if (i.validDays[aDay])
		{
			for (auto& item : i.items)
			{
				validItems.insert(item);
			}
		}
	}
	float valueLeft = aTradeValue;
	if (validItems.empty())
	{
		SYSWARNING("There are no valid items for the trader on day[" + std::to_string(aDay) + "]", "");
		return;
	}
	myItems.clear();
	while (valueLeft > 0)
	{
		ItemId itemType = *noSymBolColidePlx::select_random(validItems, Tools::RandomRange(0ULL, validItems.size() - 1));
		if (myTradePowerMap.count(itemType) == 1)
		{
			AddToInventory(itemType);
			valueLeft -= myTradePowerMap[itemType];
		}
		else
		{
			SYSERROR("Trader tried to add " + StringFromItemId(itemType) + " but it has no trade value", " is bad mkay");
		}

	}
}

void Trader::FetchTradables()
{
	myPlayerItems.clear();
	for (auto& i : myPlayerInventory->FetchItems())
	{
		if (myTradePowerMap.count(i->myItemId) != 0)
		{
			myPlayerItems.push_back({ i->myItemId,i->myAmount,0 });
		}
	}

	for (auto& item : myItems)
	{
		item.myOffered = 0;
	}
}

float Trader::EvaluateTrade()
{
	float negative = 0.f;

	for (size_t i = 0; i < myItems.size(); i++)
	{
		if (myItems[i].myOffered > 0)
		{
			if (myTradePowerMap.count(myItems[i].myItemId) == 0)
			{
				negative += 50000.f;
			}
			else
			{
				negative += myItems[i].myOffered * myTradePowerMap[myItems[i].myItemId] * myTradePowerOffer;
			}
		}
	}

	float positive = 0.f;
	for (size_t i = 0; i < myPlayerItems.size(); i++)
	{
		if (myPlayerItems[i].myOffered > 0)
		{
			positive += myPlayerItems[i].myOffered * myTradePowerMap[myPlayerItems[i].myItemId] * myTradePowerRecv;
		}
	}

	float result = positive - negative - myTradeReluctance;;
	if (result > 0)
	{
		if (!myIsAbleToTrade)
		{
			OnAbleToTrade();
		}
	}
	else
	{
		if (myIsAbleToTrade)
		{
			OnInableToTrade();
		}
	}

	float val = result;
	val /= 30.0f;
	val += 0.5f;
	val = CLAMP(0.0f, 1.0f, val);
	myFairTradeSprite->SetPosition(V2F(0.5f + LERP(-0.114f, 0.114f, val), 0.5f + 370.0f / Sprite::ourWindowSize.y));


	return result;
}

void Trader::Offer(size_t aIndex)
{
	if (aIndex > myItems.size())
	{
		return;
	}
	if (myItems[aIndex].myOffered == myItems[aIndex].myAmount)
	{
		return;
	}
	myItems[aIndex].myOffered++;
}

void Trader::OfferFromPlayer(size_t aIndex)
{
	if (aIndex > myPlayerItems.size())
	{
		return;
	}
	if (myPlayerItems[aIndex].myOffered == myPlayerItems[aIndex].myAmount)
	{
		return;
	}
	myPlayerItems[aIndex].myOffered++;
}

void Trader::RemoveOffer(size_t aIndex)
{
	if (aIndex > myItems.size())
	{
		return;
	}
	if (myItems[aIndex].myOffered == 0)
	{
		return;
	}
	myItems[aIndex].myOffered--;
}

void Trader::RemovePlayerOffer(size_t aIndex)
{
	if (aIndex > myPlayerItems.size())
	{
		return;
	}
	if (myPlayerItems[aIndex].myOffered == 0)
	{
		return;
	}
	myPlayerItems[aIndex].myOffered--;
}

void Trader::AddToInventory(ItemId aItem)
{
	for (auto& i : myItems)
	{
		if (i.myItemId == aItem)
		{
			i.myAmount++; // stack
			return;
		}
	}

	myItems.push_back({ aItem,1,0 }); // append
}

void Trader::AddToPlayerInventory(ItemId aItem)
{
	for (auto& i : myPlayerItems)
	{
		if (i.myItemId == aItem)
		{
			i.myAmount++;
			return;
		}
	}
	myItems.push_back({ aItem,1,0 });
}

bool Trader::AddTradeItemToArea(const ItemId& aItemIDType, TradeArea aToTradeArea, TradeArea aFromTradeArea)
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
	case TradeArea::PlayerOffered:
		currentVectorPtr = &myPlayerOfferedItems;
		columnSize = myPlayerTradeItemsOfferedColumns;
		firstItemPos = myPlayerTradeItemsOfferedTopLeftPos;
		break;
	case TradeArea::TraderOffered:
		currentVectorPtr = &myTraderOfferedItems;
		columnSize = myTraderTradeItemsOfferedColumns;
		firstItemPos = myTraderTradeItemsOfferedTopLeftPos;
		break;
	case TradeArea::TraderInventory:
		currentVectorPtr = &myTraderTradeItems;
		columnSize = myTraderTradeItemsColumns;
		firstItemPos = myTraderTradeItemsTopLeftPos;
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
			buttonClickToArea = TradeArea::PlayerOffered;
			break;
		case TradeArea::PlayerOffered:
			buttonClickToArea = TradeArea::PlayerInventory;
			break;
		case TradeArea::TraderOffered:
			buttonClickToArea = TradeArea::TraderInventory;
			break;
		case TradeArea::TraderInventory:
			buttonClickToArea = TradeArea::TraderOffered;
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
		switch (aFromTradeArea)
		{
		case TradeArea::PlayerInventory:
			for (int index = 0; index < myPlayerTradeItems.size(); index++)
			{
				if (aItemIDType == myPlayerTradeItems[index].myItemIDType)
				{
					OfferFromPlayer(index);
					break;
				}
			}
			break;
		case TradeArea::PlayerOffered:
			for (int index = 0; index < myPlayerTradeItems.size(); index++)
			{
				if (aItemIDType == myPlayerTradeItems[index].myItemIDType)
				{
					RemovePlayerOffer(index);
					break;
				}
			}
			break;
		case TradeArea::TraderOffered:
			for (int index = 0; index < myTraderTradeItems.size(); index++)
			{
				if (aItemIDType == myTraderTradeItems[index].myItemIDType)
				{
					RemoveOffer(index);
					break;
				}
			}
			break;
		case TradeArea::TraderInventory:
			for (int index = 0; index < myTraderTradeItems.size(); index++)
			{
				if (aItemIDType == myTraderTradeItems[index].myItemIDType)
				{
					Offer(index);
					break;
				}
			}
			break;
		default:
			SYSERROR("AddTradeItemToArea error, from area is wrong", "trade critical error");
			break;
		}
		EvaluateTrade();
		RemoveTradeItemFromArea(aItemIDType, aFromTradeArea);
	}
	return true;
}

bool Trader::RemoveTradeItemFromArea(const ItemId& aItemIDType, TradeArea aTradeArea)
{
	std::vector<TradableRepresentation>* currentVectorPtr;

	switch (aTradeArea)
	{
	case TradeArea::PlayerInventory:
		currentVectorPtr = &myPlayerTradeItems;
		break;
	case TradeArea::PlayerOffered:
		currentVectorPtr = &myPlayerOfferedItems;
		break;
	case TradeArea::TraderOffered:
		currentVectorPtr = &myTraderOfferedItems;
		break;
	case TradeArea::TraderInventory:
		currentVectorPtr = &myTraderTradeItems;
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
	return false;
}

void Trader::ClearTradeAreas()
{
	for (auto& representation : myPlayerTradeItems)
	{
		representation.myButton->RemoveAllSprites();
		myScene->RemoveText(representation.myAmountText);
		SAFE_DELETE(representation.myButton);
		SAFE_DELETE(representation.myAmountText);
	}

	myPlayerTradeItems.clear();

	for (auto& representation : myPlayerOfferedItems)
	{
		representation.myButton->RemoveAllSprites();
		myScene->RemoveText(representation.myAmountText);
		SAFE_DELETE(representation.myButton);
		SAFE_DELETE(representation.myAmountText);
	}

	myPlayerOfferedItems.clear();

	for (auto& representation : myTraderOfferedItems)
	{
		representation.myButton->RemoveAllSprites();
		myScene->RemoveText(representation.myAmountText);
		SAFE_DELETE(representation.myButton);
		SAFE_DELETE(representation.myAmountText);
	}

	myTraderOfferedItems.clear();

	for (auto& representation : myTraderTradeItems)
	{
		representation.myButton->RemoveAllSprites();
		myScene->RemoveText(representation.myAmountText);
		SAFE_DELETE(representation.myButton);
		SAFE_DELETE(representation.myAmountText);
	}

	myTraderTradeItems.clear();
}
