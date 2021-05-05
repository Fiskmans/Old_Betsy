#pragma once
#include "Component.h"

class Inventory;
class Scene;
class TextFactory;
class SpriteFactory;
class SpriteInstance;
class Button;

struct LootTable
{
	std::bitset<AMOUNTOFDAYS> validDays;
	std::vector<ItemId> items;
};

enum class TradeArea
{
	None,
	PlayerInventory,
	PlayerOffered,
	TraderOffered,
	TraderInventory
};

struct TradableRepresentation
{
	ItemId myItemIDType;
	TradeArea myTradeArea = TradeArea::None;
	Button* myButton = nullptr;
	int myAmount = 0;
	TextInstance* myAmountText = nullptr;
};

class Trader :
	public Component
{
public:

	virtual void InteractWith(Item* aHeldItem) override;

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetPlayerInventory(Inventory* aInventory);
	void PrepareUI(Scene* aScenePtr, SpriteFactory* aSpriteFactoryPtr, TextFactory* aTextFactoryPtr);

	void StopTrading();
private:
	void StartTrading();
	void DisplayTradeWindow();
	void CloseTradeWindow();
	void OnAbleToTrade();
	void OnInableToTrade();

	void DoTrade();

	void GenerateInventory(float aTradeValue,char aDay);

	void FetchTradables();

	std::vector<LootTable> myLootTables;

	GAMEMETRIC(float, myTradeReluctance, TRADERDEFAULTSTANCE, 0.5f);
	GAMEMETRIC(float, myTradePowerOffer, TRADEROFFERPOWER, 1.2f);
	GAMEMETRIC(float, myTradePowerRecv, TRADERRECIEVEPOWER, 0.9f);

	float EvaluateTrade();
	
	void Offer(size_t aIndex);
	void OfferFromPlayer(size_t aIndex);
	void RemoveOffer(size_t aIndex);
	void RemovePlayerOffer(size_t aIndex);
	void AddToInventory(ItemId aItem);
	void AddToPlayerInventory(ItemId aItem);
	bool myIsTrading = false;
	bool myIsAbleToTrade = false;

	Inventory* myPlayerInventory = nullptr;

	struct Tradable
	{
		ItemId myItemId;
		size_t myAmount;
		size_t myOffered;
	};

	std::vector<Tradable> myItems;
	std::vector<Tradable> myPlayerItems;
	std::map<ItemId, float> myTradePowerMap;

	Scene* myScene;
	TextFactory* myTextFactory;
	SpriteFactory* mySpriteFactory;

	SpriteInstance* myTradeBlackBackground;
	SpriteInstance* myTradeWindowBackground;
	SpriteInstance* myFairTradeSprite;

	Button* myAcceptTradeButton;
	Button* myCancelTradeButton;

	std::vector<TradableRepresentation> myPlayerTradeItems;
	std::vector<TradableRepresentation> myPlayerOfferedItems;
	std::vector<TradableRepresentation> myTraderTradeItems;
	std::vector<TradableRepresentation> myTraderOfferedItems;

	V2F myPlayerTradeItemsTopLeftPos;
	V2F myPlayerTradeItemsOfferedTopLeftPos;
	V2F myTraderTradeItemsOfferedTopLeftPos;
	V2F myTraderTradeItemsTopLeftPos;

	float myTradeItemsXOffset;
	float myTradeItemsYOffset;

	int myPlayerTradeItemsColumns;
	int myPlayerTradeItemsOfferedColumns;
	int myTraderTradeItemsColumns;
	int myTraderTradeItemsOfferedColumns;

	bool AddTradeItemToArea(const ItemId& aItemIDType, TradeArea aToTradeArea, TradeArea aFromTradeArea);
	bool RemoveTradeItemFromArea(const ItemId& aItemIDType, TradeArea aTradeArea);
	
	void ClearTradeAreas();
};

