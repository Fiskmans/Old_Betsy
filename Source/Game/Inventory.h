#pragma once
#include "Component.h"
#include "Observer.hpp"
#include "AABB2D.h"

#define INVENTORYSIZE 30
#define TOOLBARSIZE 10

class Item;
class Scene;
class TextFactory;
class SpriteFactory;
class SpriteInstance;

class Inventory : public Component, public Observer
{
public:
	Inventory();
	~Inventory();

	virtual void Init(Entity* aEntity) override;
	virtual void Init(Entity* aEntity, TextFactory* aTextFactory, SpriteFactory* aSpriteFactory, Scene* aScene);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	virtual void SubscribeToMyMessages() final override;
	virtual void UnSubscribeToMyMessages() final override;

	bool Add(Item& anItem, const bool aShouldStack = true);
	bool Remove(const Item& anItem, const bool aShouldDelete = true);
	bool Replace(Item& anItemToReplace, Item& aNewItem, const bool aShouldStack = true);

	bool IsToggled() const;

	void AddToScene();
	void RemoveFromScene();
	void AddToolbarToScene();
	void RemoveToolbarFromScene();

	Item* GetSelectedItem();
	std::vector<Item*> FetchItems();

	static const std::string GetItemIconPath(ItemId aItemIDType);

private:

#if USEIMGUI // if we get a bar for wateringcan include this always
	GAMEMETRIC(int, myWaterCanSize, PLAYERWATERINGCANSIZE, 5);
#endif // USEIMGUI

	virtual void RecieveMessage(const Message& aMessage) override;

	struct Slot
	{
		V2F position;
		Item item;
		TextInstance* amountNumber = nullptr;
		TextInstance* tooltip = nullptr;
		SpriteInstance* icon = nullptr;
	};

	static std::map<ItemId, std::string> myIconLookup;

	std::array<Slot, INVENTORYSIZE> myInventory;
	std::array<V2F, TOOLBARSIZE> myToolbarPositions;
	CU::AABB2D<float> myBounds;
	CU::AABB2D<float> myToolbarBounds;
	Scene* myScenePtr;

	SpriteFactory* mySpriteFactory;
	SpriteInstance* mySlotMarker;

	SpriteInstance* myItemTip;
	bool myItemTipActive;
	float myItemTipTimer;
	float myItemTipTime;

	int mySize;
	char myClickedSlot;

	char mySelectedSlot;      
	bool myIsToggled;

	Slot* GetFirstEmpty();
	Slot* Find(const Item& anItem);
	bool Switch(Slot* aSlot, Slot* anotherSlot);
	bool Remove(Slot* aSlot, const bool aShouldDelete = true);

	bool Stack(Item& anItem);
	bool IsEmpty(const Slot& aSlot) const;
	char GetIndexFromMouse(V2F aMousePos) const;
	char GetToolBarIndexFromMouse(V2F aMousePos) const;
	void PopulateIconLookup(const std::string& aFilePath);
	void SetToolTip(Slot& aSlot);
};