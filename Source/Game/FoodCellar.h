#pragma once
#include "Component.h"
#include "Trader.h"

class TextFactory;
class SpriteFactory;
class Button;


class FoodCellar : public Component
{
public:
	virtual void InteractWith(Item* aHeldItem) override;

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	void SetUp(Scene* aScene, TextFactory* aTextFactory, SpriteFactory* aSpriteFactory, Entity* aPlayerPtr);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	static void PopulateCalorieLookup();
	std::unordered_map<ItemIdType, int> GetItemsAndAmounts();
	double GetStoredTotalCalories();
	void SetPtrs();
	void SetPlayerInventory(Inventory* aInventory);
private:
	static bool CanStore(ItemId aItem);
	static void Store(ItemId aItem);
	static void RemoveFromStorage(ItemId aItem);
	static double ourStoredCalories;
	static std::unordered_map<ItemIdType, double> myCalorieLookupMap;
	static std::unordered_map<ItemIdType, int> myTypeAmounts;

	Inventory* myPlayerInventory;

	Entity* myPlayer;
	Scene* myScene;
	TextFactory* myTextFactory;
	SpriteFactory* mySpriteFactory;

	SpriteInstance* myStorageBlackBackground;
	SpriteInstance* myStorageWindowBackground;
	SpriteInstance* myStoredAmountSprite;
	SpriteInstance* myStoredAmountChangeSprite;

	TextInstance* myStoredAmountText;

	Button* myCloseStorageButton;

	std::vector<TradableRepresentation> myPlayerTradeItems;
	std::vector<TradableRepresentation> myStorageTradeItems;

	V2F myPlayerTradeItemsTopLeftPos;
	V2F myStorageTradeItemsTopLeftPos;

	float myTradeItemsXOffset;
	float myTradeItemsYOffset;

	int myPlayerTradeItemsColumns;
	int myTraderTradeItemsColumns;

	float myCaloriesBarXPosition;
	float myCaloriesBarXScaleForMaxFill;

	void DisplayStorageMenu();
	void CloseStorageMenu();

	bool myIsOpen = false;

	bool AddTradeItemToArea(const ItemId& aItemIDType, TradeArea aToTradeArea, TradeArea aFromTradeArea);
	bool RemoveTradeItemFromArea(const ItemId& aItemIDType, TradeArea aTradeArea);

	void ClearTradeAreas();
};

