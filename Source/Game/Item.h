#pragma once

typedef size_t ItemIdType;

struct ItemId
{
	ItemIdType ID;
	size_t stackSize = 1;

	ItemId() = default;
	ItemId(const ItemId& anID) = default;

	bool operator==(const ItemId& rhs) const
	{
		return (ID == rhs.ID /*&& stackSize == rhs.stackSize*/);
	}

	bool operator!=(const ItemId& rhs) const
	{
		return !((*this) == rhs);
	}

	bool operator<(const ItemId& rhs) const
	{
		return (ID < rhs.ID);
	}

	ItemId& operator=(const ItemId& rhs)
	{
		ID = rhs.ID;
		stackSize = rhs.stackSize;
		return (*this);
	}

};

namespace std
{
	template<>
	struct hash<ItemId>
	{
		std::size_t operator()(const ItemId& k) const
		{
			return hash<ItemIdType>()(k.ID) + ~std::hash<size_t>()(k.stackSize);
		}
	};
}

class SpriteInstance;

const ItemId& ItemIdFromString(const std::string& aItemName);
const ItemId& ItemFromID(ItemIdType aID);
const std::string& StringFromItemId(ItemId aID);

void RegisterItemLookup(const std::string& aItemName, ItemId aID);
const ItemId& RegisterItem(const std::string& aItemName, int aStackSize);

void ItemImgui();

struct ItemCreationStruct
{
	ItemIdType myItemId;
	int myAmount = 1;
	V3F myPosition;
};

namespace PreCalculatedItemIds
{
#pragma push(ITEMID)
#define ITEMID(name, stackSize) const ItemId name = RegisterItem(#name, stackSize);

	ITEMID(None, 0);
	ITEMID(Hoe, 1);
	ITEMID(Scissor, 1);
	ITEMID(Seeds, 50);
	ITEMID(WateringCan, 1);
	ITEMID(WateringCanFilled, 1);
	ITEMID(Basket, 1);
	ITEMID(Wool, 100);
	ITEMID(Chicken, 1);
	ITEMID(Eggs, 50);

#pragma pop(ITEMID)
}

class Item
{
public:
	ItemId myItemId = PreCalculatedItemIds::None;

	bool UseCharge();
	bool Consume();

	size_t myCharges = 1;
	size_t myAmount = 1;
	size_t& myMaxAmount = myItemId.stackSize;

	bool operator==(const Item& another) const
	{
		return (myItemId == another.myItemId && myCharges == another.myCharges && myAmount == another.myAmount && myMaxAmount == another.myMaxAmount);
	}

	Item& operator=(const Item& another)
	{
		myItemId = another.myItemId;
		myCharges = another.myCharges;
		myAmount = another.myAmount;
		myMaxAmount = myItemId.stackSize;

		return (*this);
	}
};

inline std::unordered_map<std::string, ItemId>& StringToItemIdMap()
{
	static std::unordered_map<std::string, ItemId> map;
	return map;
}

inline const ItemId& ItemIdFromString(const std::string& aItemName)
{
	if (StringToItemIdMap().count(aItemName) == 0)
	{
		RegisterItem(aItemName, 1);
	}

	return StringToItemIdMap()[aItemName];
}

inline const ItemId& ItemFromID(ItemIdType aID)
{
	for (auto& item : StringToItemIdMap())
	{
		if (item.second.ID == aID)
		{
			return item.second;
		}
	}

	return PreCalculatedItemIds::None;
}

inline std::unordered_map<size_t, std::string>& ItemIdToStringMap()
{
	static  std::unordered_map<size_t, std::string> map;
	return map;
}

inline const std::string& StringFromItemId(ItemId aID)
{
	return ItemIdToStringMap()[aID.ID];
}

inline void RegisterItemLookup(const std::string& aItemName, ItemId aID)
{
	ItemIdToStringMap()[aID.ID] = aItemName;
}

inline const ItemId& RegisterItem(const std::string& aItemName, int aStackSize)
{
	if (StringToItemIdMap().count(aItemName) == 0)
	{
		StringToItemIdMap()[aItemName].ID = StringToItemIdMap().size();
		RegisterItemLookup(aItemName, StringToItemIdMap()[aItemName]);
	}

	StringToItemIdMap()[aItemName].stackSize = aStackSize;

	return 	StringToItemIdMap()[aItemName];
}

