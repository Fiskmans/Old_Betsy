#include "pch.h"
#include "Inventory.h"
#include "SpriteInstance.h"
#include "TextInstance.h"
#include "TextFactory.h"
#include "SpriteFactory.h"
#include "FiskJSON.h"
#include "Item.h"
#include "TimeHandler.h"
#include "Entity.h";
#include "AnimationComponent.h"
#include "PlayerController.h"
#include "AssetManager.h"

#define NUMBERTEXTOFFSET V2F(0.004f, 0.003f)
#define TOOLTIPOFFSET V2F(0.004f, 0.00f)

std::map<ItemId, std::string> Inventory::myIconLookup;

Inventory::Inventory() :
	mySize(0),
	myIsToggled(false),
	myScenePtr(nullptr),
	mySlotMarker(0),
	mySelectedSlot(0),
	myClickedSlot(-1)
{
	myInventory.fill(Slot());
}

Inventory::~Inventory()
{
	Reset();
}

void Inventory::Init(Entity* aEntity)
{
}

void Inventory::Init(Entity* aEntity, TextFactory* aTextFactory, SpriteFactory* aSpriteFactory, Scene* aScene)
{
	static_assert(1ULL << (sizeof(mySelectedSlot) * CHAR_BIT) - 1 > INVENTORYSIZE, "Slotselectiontype is too small for inventorysize");

	SubscribeToMyMessages();

	if (myIconLookup.size() == 0)
	{
		PopulateIconLookup("data/ItemIconBindings.json");
	}

	myEntity = aEntity;
	myScenePtr = aScene;

	for (auto& slot : myInventory)
	{
		slot.amountNumber = aTextFactory->CreateText();
		slot.amountNumber->SetColor(V4F(1.f, 0.5f, 0.f, 1.f));

		slot.tooltip = aTextFactory->CreateToolTip("ui/ToolTipBackground.dds", V2F(10.f, 10.f), "", "toolTip.spritefont");
		slot.tooltip->SetColor(V4F(1.f, 1.f, 1.f, 1.f));
	}

	SpriteInstance* toolbarBack = aSpriteFactory->CreateSprite("ui/toolbar/background.dds");
	V2F start = V2F(0.5f, 1.f) - toolbarBack->GetSizeOnScreen() * V2F(0.442f, 0.53f);
	const V2F cellSize = V2F(toolbarBack->GetSizeOnScreen().x * 0.985f / TOOLBARSIZE, 0.f);
	for (size_t i = 0; i < TOOLBARSIZE; i++)
	{
		myToolbarPositions[i] = start + cellSize * CAST(float, i);
	}
	myToolbarBounds = CU::AABB2D<float>(V2F(0.5f, 1.f) - toolbarBack->GetSizeOnScreen() * V2F(0.5f, 0.85f), start + toolbarBack->GetSizeOnScreen() * V2F(0.94f, 0.3f));

	SpriteInstance* invnBack = aSpriteFactory->CreateSprite("ui/inventory/background.dds");
	V2F size = invnBack->GetSizeOnScreen();
	start = V2F(0.5f, 0.5f) - size * V2F(0.45f, 0.2f);
	myBounds = CU::AABB2D<float>(V2F(0.5f, 0.5f) - size * V2F(0.49f, 0.31f), V2F(0.5f, 0.5f) + size * V2F(0.49f, 0.3f));
	size = (myBounds.Max() - myBounds.Min()) / V2F(TOOLBARSIZE, INVENTORYSIZE / TOOLBARSIZE);
	for (size_t i = 0; i < INVENTORYSIZE; i++)
	{
		myInventory[i].position = start + V2F(size.x * (i % TOOLBARSIZE), size.y * (i / TOOLBARSIZE));
	}


	mySlotMarker = aSpriteFactory->CreateSprite("ui/UIButtons/Selected.dds");
	mySlotMarker->SetPosition(myToolbarPositions[0]);
	mySlotMarker->SetDepth(0.05f);
	mySlotMarker->SetPivot({ 0.5,0.5 });

	myItemTip = aSpriteFactory->CreateSprite("ui/UIButtons/Selected.dds");
	myItemTip->SetPosition(myToolbarPositions[0]);
	myItemTip->SetDepth(0.05f);
	myItemTip->SetPivot({ 0.5,0.5 });

	myItemTipTime = 1;

	mySpriteFactory = aSpriteFactory;

	SAFE_DELETE(toolbarBack);
	SAFE_DELETE(invnBack);
}

void Inventory::Update(const float aDeltaTime)
{
#if USEIMGUI
	WindowControl::Window("Inventory",
		[this]()
		{
			for (size_t i = 0; i < INVENTORYSIZE; i++)
			{
				ImGui::PushID(i);
				bool selected = i == mySelectedSlot;
				if (!IsEmpty(myInventory[i]))
				{
					if (myInventory[i].item.myAmount > 1)
					{
						if (ImGui::Selectable((StringFromItemId(myInventory[i].item.myItemId) + ": " + std::to_string(myInventory[i].item.myAmount)).c_str(), &selected))
						{
							if (selected)
							{
								mySelectedSlot = i;
							}
						}
					}
					else
					{
						if (ImGui::Selectable(StringFromItemId(myInventory[i].item.myItemId).c_str(), &selected))
						{
							if (selected)
							{
								mySelectedSlot = i;
							}
						}
					}
					if (myInventory[i].item.myItemId == PreCalculatedItemIds::WateringCan)
					{
						ImGui::SameLine();
						ImGui::ProgressBar(float(myInventory[i].item.myCharges) / float(myWaterCanSize));
					}
				}
				else
				{
					if (ImGui::Selectable("Empty", &selected))
					{
						if (selected)
						{
							mySelectedSlot = i;
						}
					}
				}
				ImGui::PopID();
			}
		});
#endif

	for (auto& slot : myInventory)
	{
		if (!IsEmpty(slot))
		{
			if (slot.item.myAmount == 0)
			{
				Remove(&slot);
				continue;
			}

			if (slot.item.myMaxAmount > 1)
			{
				slot.amountNumber->SetText(std::to_wstring(slot.item.myAmount));
			}
		}
	}

	if (myItemTipActive)
	{
		if (myItemTipTimer > myItemTipTime * 0.75f)
		{
			myScenePtr->AddSprite(myItemTip);
		}
		else if (myItemTipTimer > myItemTipTime * 0.5f)
		{
			myScenePtr->RemoveSprite(myItemTip);
		}
		else if (myItemTipTimer > myItemTipTime * 0.25f)
		{
			myScenePtr->AddSprite(myItemTip);
		}
		else
		{
			myScenePtr->RemoveSprite(myItemTip);
			myItemTipActive = false;
		}

		myItemTipTimer -= aDeltaTime;
	}

	if (myIsToggled)
	{
		DebugDrawer::GetInstance().Draw2DBoundingBox(myBounds);
	}
	else
	{
		DebugDrawer::GetInstance().Draw2DBoundingBox(myToolbarBounds);
	}
}

void Inventory::Reset()
{
	myEntity = nullptr;
	for (auto& slot : myInventory)
	{
		SAFE_DELETE(slot.amountNumber);
		SAFE_DELETE(slot.icon);
	}
	mySize = 0;
	myInventory.fill(Slot());
	myToolbarPositions.fill(V2F());
	UnSubscribeToMyMessages();
}

void Inventory::SubscribeToMyMessages()
{
	SubscribeToMessage(MessageType::InputInteractHit);
	SubscribeToMessage(MessageType::SpawnItem);
	SubscribeToMessage(MessageType::InputScrollUp);
	SubscribeToMessage(MessageType::InputScrollDown);
	SubscribeToMessage(MessageType::InputToggleInventory);
	SubscribeToMessage(MessageType::InputLeftMouseHit);
	SubscribeToMessage(MessageType::InputLeftMouseReleased);
	SubscribeToMessage(MessageType::InputLeftMouseDown);
	SubscribeToMessage(MessageType::InputMouseMoved);

	SubscribeToMessage(MessageType::InputEquipmentSlot1Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot2Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot3Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot4Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot5Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot6Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot7Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot8Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot9Hit);
	SubscribeToMessage(MessageType::InputEquipmentSlot10Hit);

	SubscribeToMessage(MessageType::ShouldUseItem);
	SubscribeToMessage(MessageType::LockPlayer);
	SubscribeToMessage(MessageType::UnlockPlayer);
	SubscribeToMessage(MessageType::TradeStarted);
	SubscribeToMessage(MessageType::TradeEnded);
}

void Inventory::UnSubscribeToMyMessages()
{
	UnSubscribeToMessage(MessageType::InputInteractHit);
	UnSubscribeToMessage(MessageType::SpawnItem);
	UnSubscribeToMessage(MessageType::InputScrollUp);
	UnSubscribeToMessage(MessageType::InputScrollDown);
	UnSubscribeToMessage(MessageType::InputToggleInventory);
	UnSubscribeToMessage(MessageType::InputLeftMouseHit);
	UnSubscribeToMessage(MessageType::InputLeftMouseReleased);
	UnSubscribeToMessage(MessageType::InputLeftMouseDown);
	UnSubscribeToMessage(MessageType::InputMouseMoved);

	UnSubscribeToMessage(MessageType::InputEquipmentSlot1Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot2Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot3Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot4Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot5Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot6Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot7Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot8Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot9Hit);
	UnSubscribeToMessage(MessageType::InputEquipmentSlot10Hit);

	UnSubscribeToMessage(MessageType::ShouldUseItem);
	UnSubscribeToMessage(MessageType::LockPlayer);
	UnSubscribeToMessage(MessageType::UnlockPlayer);
	UnSubscribeToMessage(MessageType::TradeStarted);
	UnSubscribeToMessage(MessageType::TradeEnded);
}

bool Inventory::Add(Item& anItem, const bool aShouldStack)
{
	if (anItem.myMaxAmount > 1 && aShouldStack)
	{
		Stack(anItem);

		if (anItem.myAmount <= 0)
		{
			return true;
		}
	}

	Slot* slot = GetFirstEmpty();

	if (slot)
	{
		mySize++;
		slot->item = anItem;

		Item& item = slot->item;

		if (item.myAmount > item.myMaxAmount)
		{
			Item newItem = item;
			newItem.myAmount = item.myAmount - item.myMaxAmount;
			item.myAmount = item.myMaxAmount;

			Add(newItem, aShouldStack);
		}

		if (item.myMaxAmount > 1)
		{
			slot->amountNumber->SetText(std::to_wstring(item.myAmount));
			myScenePtr->AddText(slot->amountNumber);
		}

		if (slot->icon)
		{
			if (slot->icon->HasBeenAddedToScene())
			{
				myScenePtr->RemoveSprite(slot->icon);
			}
			delete slot->icon;
		}

		slot->icon = mySpriteFactory->CreateSprite(myIconLookup[item.myItemId]);
		slot->icon->SetPivot({ 0.5,0.5 });
		SetToolTip(*slot);

		if (myIsToggled)
		{
			slot->icon->SetPosition(slot->position);
			myScenePtr->AddSprite(slot->icon);
		}
		else
		{
			RemoveToolbarFromScene();
			AddToolbarToScene();
		}


		return true;
	}

	return false;
}

bool Inventory::Remove(const Item& anItem, const bool aShouldDelete)
{
	return Remove(Find(anItem));
}

bool Inventory::Replace(Item& anItemToReplace, Item& aNewItem, const bool aShouldStack)
{
	if (Remove(anItemToReplace))
	{
		if (Add(aNewItem))
		{
			return true;
		}
	}

	return false;
}

bool Inventory::Remove(Slot* aSlot, const bool aShouldDelete)
{
	if (aSlot)
	{
		if (aSlot->icon && aSlot->icon->HasBeenAddedToScene())
		{
			myScenePtr->RemoveSprite(aSlot->icon);
		}

		if (aShouldDelete)
		{
			SAFE_DELETE(aSlot->icon);
		}

		aSlot->item = Item();
		aSlot->amountNumber->SetText(L"");
		aSlot->tooltip->SetText(L"");
		aSlot->tooltip->SetTitle(L"");
		mySize--;

		return true;
	}

	return false;
}

bool Inventory::Stack(Item& anItem)
{
	Slot* curr = nullptr;

	for (auto& slot : myInventory)
	{
		if (slot.item.myItemId == anItem.myItemId && slot.item.myAmount < slot.item.myMaxAmount)
		{
			curr = &slot;
			break;
		}
	}

	if (!curr)
	{
		return false;
	}

	Item& item = curr->item;

	item.myAmount = item.myAmount + anItem.myAmount;
	anItem.myAmount = MAX(0, CAST(int, item.myAmount) - CAST(int, item.myMaxAmount));
	item.myAmount = MIN(item.myAmount, item.myMaxAmount);

	curr->amountNumber->SetText(std::to_wstring(item.myAmount));

	if (anItem.myAmount > 0)
	{
		Stack(anItem);
	}

	return true;
}

bool Inventory::IsEmpty(const Slot& aSlot) const
{
	return aSlot.item.myItemId == PreCalculatedItemIds::None;
}

char Inventory::GetIndexFromMouse(V2F aMousePos) const
{
	if (myBounds.IsInside(aMousePos))
	{
		aMousePos -= myBounds.Min();
		const V2F size = myBounds.Max() - myBounds.Min();

#ifdef  _DEBUG
		const float x = aMousePos.x / size.x;
		const float y = aMousePos.y / size.y;
#endif //  _DEBUG

		const float col = floor((aMousePos.x / size.x) * TOOLBARSIZE);
		const float row = floor((aMousePos.y / size.y) * INVENTORYSIZE / TOOLBARSIZE);

		const char slot = row * TOOLBARSIZE + col;
		if (slot < INVENTORYSIZE)
		{
			return slot;
		}
	}

	return -1;
}

char Inventory::GetToolBarIndexFromMouse(V2F aMousePos) const
{
	if (myToolbarBounds.IsInside(aMousePos))
	{
		aMousePos -= myToolbarBounds.Min();
		const V2F size = myToolbarBounds.Max() - myToolbarBounds.Min();

#ifdef  _DEBUG
		const float x = aMousePos.x / size.x;
		const float y = aMousePos.y / size.y;
#endif //  _DEBUG

		const float slot = floor((aMousePos.x / size.x) * TOOLBARSIZE);

		if (slot < TOOLBARSIZE)
		{
			return slot;
		}
	}

	return -1;
}


bool Inventory::IsToggled() const
{
	return myIsToggled;
}


void Inventory::AddToScene()
{
	for (auto& slot : myInventory)
	{
		if (!IsEmpty(slot) && slot.icon)
		{
			slot.icon->SetPosition(slot.position);
			myScenePtr->AddSprite(slot.icon);
			slot.amountNumber->SetPosition(slot.position + NUMBERTEXTOFFSET);
			slot.tooltip->SetPosition(slot.position + TOOLTIPOFFSET);
			myScenePtr->AddText(slot.amountNumber);
		}
	}
}

void Inventory::RemoveFromScene()
{
	for (auto& slot : myInventory)
	{
		if (!IsEmpty(slot) && slot.icon)
		{
			myScenePtr->RemoveSprite(slot.icon);
			myScenePtr->RemoveText(slot.amountNumber);
		}
	}
}

void Inventory::AddToolbarToScene()
{
	for (size_t i = 0; i < TOOLBARSIZE; i++)
	{
		if (!IsEmpty(myInventory[i]) && myInventory[i].icon)
		{
			myInventory[i].icon->SetPosition(myToolbarPositions[i]);
			myScenePtr->AddSprite(myInventory[i].icon);
			myInventory[i].amountNumber->SetPosition(myToolbarPositions[i] + NUMBERTEXTOFFSET);
			myInventory[i].tooltip->SetPosition(myToolbarPositions[i] + TOOLTIPOFFSET);
			myScenePtr->AddText(myInventory[i].amountNumber);
		}
	}

	myScenePtr->AddSprite(mySlotMarker);
}

void Inventory::RemoveToolbarFromScene()
{
	for (size_t i = 0; i < TOOLBARSIZE; i++)
	{
		if (!IsEmpty(myInventory[i]) && myInventory[i].icon)
		{
			myScenePtr->RemoveSprite(myInventory[i].icon);
			myScenePtr->RemoveText(myInventory[i].amountNumber);
		}
	}

	myScenePtr->RemoveSprite(mySlotMarker);
}

Item* Inventory::GetSelectedItem()
{
	if (myInventory[mySelectedSlot].item.myItemId != PreCalculatedItemIds::None)
	{
		return &myInventory[mySelectedSlot].item;
	}

	return nullptr;
}

std::vector<Item*> Inventory::FetchItems()
{
	std::vector<Item*> out;
	for (auto& i : myInventory)
	{
		if (i.item.myItemId != PreCalculatedItemIds::None)
		{
			out.push_back(&i.item);
		}
	}
	return out;
}

const std::string Inventory::GetItemIconPath(ItemId aItemIDType)
{
	return myIconLookup[aItemIDType];
}

void Inventory::RecieveMessage(const Message& aMessage)
{
	static int hoveredSlot = -1;
	static bool playerLocked = false;
	int selectSlot = mySelectedSlot;

	switch (aMessage.myMessageType)
	{
	case  MessageType::InputToggleInventory:
	{
		myIsToggled = !myIsToggled;
		myClickedSlot = -1;

		if (myIsToggled)
		{
			TimeHandler::GetInstance().PauseTime();
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
			myEntity->GetComponent<PlayerController>()->StopMoving();
		}
		else
		{
			TimeHandler::GetInstance().ResumeTime();
		}

		Message mess;
		mess.myMessageType = MessageType::InventoryToggled;
		mess.myBool = myIsToggled;
		PostMaster::GetInstance()->SendMessages(mess);
	}
	break;

	case MessageType::SpawnItem:
	{
		ItemCreationStruct& incoming = *reinterpret_cast<ItemCreationStruct*>(aMessage.myData);
		Item item;
		item.myItemId = ItemFromID(incoming.myItemId);
		item.myAmount = incoming.myAmount;

		Add(item);
	}
	break;

	case MessageType::InputScrollUp:
		--mySelectedSlot;
		mySelectedSlot += TOOLBARSIZE;
		mySelectedSlot %= TOOLBARSIZE;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;

	case MessageType::InputScrollDown:
		++mySelectedSlot;
		mySelectedSlot %= TOOLBARSIZE;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;

	case MessageType::InputLeftMouseHit:
		if (myIsToggled)
		{
			myClickedSlot = GetIndexFromMouse(V2F(aMessage.myFloatValue, aMessage.myFloatValue2));

			if (myClickedSlot >= 0 && IsEmpty(myInventory[myClickedSlot]))
			{
				myClickedSlot = -1;
			}

			if (hoveredSlot >= 0)
			{
				myScenePtr->RemoveText(myInventory[hoveredSlot].tooltip);
				hoveredSlot = -1;
			}
		}
		break;

	case MessageType::InputLeftMouseReleased:
		if (myIsToggled && myClickedSlot >= 0)
		{
			myInventory[myClickedSlot].icon->SetPosition(myInventory[myClickedSlot].position);
			myInventory[myClickedSlot].amountNumber->SetPosition(myInventory[myClickedSlot].position + NUMBERTEXTOFFSET);

			char releasedSlot = GetIndexFromMouse(V2F(aMessage.myFloatValue, aMessage.myFloatValue2));

			if (releasedSlot >= 0 && releasedSlot != myClickedSlot)
			{
				Switch(&myInventory[myClickedSlot], &myInventory[releasedSlot]);
			}
		}

		myClickedSlot = -1;
		break;

	case MessageType::InputLeftMouseDown:
		if (myClickedSlot >= 0 && myInventory[myClickedSlot].icon)
		{
			myInventory[myClickedSlot].icon->SetPosition(V2F(aMessage.myFloatValue, aMessage.myFloatValue2));
			myInventory[myClickedSlot].amountNumber->SetPosition(V2F(aMessage.myFloatValue, aMessage.myFloatValue2) + NUMBERTEXTOFFSET);
		}
		break;

		int index;
	case MessageType::InputMouseMoved:

		if (myClickedSlot == -1 && !playerLocked)
		{
			if (myIsToggled)
			{
				index = GetIndexFromMouse(V2F(aMessage.myFloatValue, aMessage.myFloatValue2));
			}
			else
			{
				index = GetToolBarIndexFromMouse(V2F(aMessage.myFloatValue, aMessage.myFloatValue2));
			}

			if (index != hoveredSlot)
			{
				static std::vector<TextInstance*> hiddenTexts;

				for (auto& i : hiddenTexts)
				{
					myScenePtr->AddText(i);
				}

				hiddenTexts.clear();

				if (hoveredSlot >= 0)
				{
					myScenePtr->RemoveText(myInventory[hoveredSlot].tooltip);
				}
				if (index >= 0)
				{
					myScenePtr->AddText(myInventory[index].tooltip);

					CU::AABB2D<float> bound;
					bound.InitWithMinAndMax(myInventory[index].tooltip->GetPosition(), myInventory[index].tooltip->GetPosition() + myInventory[index].tooltip->GetBackgroundSize());

					for (auto& i : myInventory)
					{
						if (i.item.myMaxAmount > 1 && bound.IsInside(i.amountNumber->GetPosition()))
						{
							myScenePtr->RemoveText(i.amountNumber);

							hiddenTexts.push_back(i.amountNumber);
						}
					}

				}

				hoveredSlot = index;
			}
		}

		break;

	case MessageType::InputEquipmentSlot1Hit:
		mySelectedSlot = 0;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot2Hit:
		mySelectedSlot = 1;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot3Hit:
		mySelectedSlot = 2;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot4Hit:
		mySelectedSlot = 3;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot5Hit:
		mySelectedSlot = 4;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot6Hit:
		mySelectedSlot = 5;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot7Hit:
		mySelectedSlot = 6;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot8Hit:
		mySelectedSlot = 7;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot9Hit:
		mySelectedSlot = 8;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;
	case MessageType::InputEquipmentSlot10Hit:
		mySelectedSlot = 9;
		mySlotMarker->SetPosition(myToolbarPositions[mySelectedSlot]);
		break;


	case MessageType::ShouldUseItem:
		for (int i = 0; i < myToolbarPositions.size(); ++i)
		{
			if (myInventory[i].item.myItemId.ID == aMessage.myIntValue)
			{
				myScenePtr->AddSprite(myItemTip);
				myItemTip->SetPosition(myToolbarPositions[i]);
				myItemTipActive = true;
				myItemTipTimer = myItemTipTime;
				break;
			}
		}
		break;

	case MessageType::LockPlayer:
		playerLocked = true;
		break;

	case MessageType::UnlockPlayer:
		playerLocked = false;
		break;

	case MessageType::TradeStarted:
		RemoveToolbarFromScene();
			break;

	case MessageType::TradeEnded:
		AddToolbarToScene();
		break;

	default:
		break;
	}

	if (selectSlot != mySelectedSlot)
	{
		PostMaster::GetInstance()->SendMessages(MessageType::ChangedItem);
	}
}

Inventory::Slot* Inventory::GetFirstEmpty()
{
	if (mySize < INVENTORYSIZE)
	{
		for (auto& slot : myInventory)
		{
			if (IsEmpty(slot))
			{
				return &slot;
			}
		}
	}

	return nullptr;
}

Inventory::Slot* Inventory::Find(const Item& anItem)
{
	if (mySize > 0)
	{
		for (auto& slot : myInventory)
		{
			if (slot.item == anItem)
			{
				return &slot;
			}
		}
	}

	return nullptr;
}

bool Inventory::Switch(Slot* aSlot, Slot* anotherSlot)
{
	if (!aSlot || !anotherSlot)
	{
		return false;
	}

	if (anotherSlot->item.myItemId == aSlot->item.myItemId && anotherSlot->item.myCharges == aSlot->item.myCharges)
	{
		anotherSlot->item.myAmount = aSlot->item.myAmount + anotherSlot->item.myAmount;
		aSlot->item.myAmount = MAX(0, CAST(int, anotherSlot->item.myAmount) - CAST(int, anotherSlot->item.myMaxAmount));
		anotherSlot->item.myAmount = MIN(anotherSlot->item.myAmount, anotherSlot->item.myMaxAmount);

		anotherSlot->amountNumber->SetText(std::to_wstring(anotherSlot->item.myAmount));
		anotherSlot->icon->SetPosition(anotherSlot->position);
		anotherSlot->amountNumber->SetPosition(anotherSlot->position + NUMBERTEXTOFFSET);

		if (aSlot->item.myAmount < 1)
		{
			Remove(aSlot);
		}

		return true;
	}

	Item itemCopy = aSlot->item;
	aSlot->item = anotherSlot->item;
	anotherSlot->item = itemCopy;

	SpriteInstance* iconCopy = aSlot->icon;
	aSlot->icon = anotherSlot->icon;
	anotherSlot->icon = iconCopy;

	TextInstance* textCopy = aSlot->amountNumber;
	aSlot->amountNumber = anotherSlot->amountNumber;
	anotherSlot->amountNumber = textCopy;

	textCopy = aSlot->tooltip;
	aSlot->tooltip = anotherSlot->tooltip;
	anotherSlot->tooltip = textCopy;

	aSlot->amountNumber->SetPosition(aSlot->position + NUMBERTEXTOFFSET);
	anotherSlot->amountNumber->SetPosition(anotherSlot->position + NUMBERTEXTOFFSET);
	aSlot->tooltip->SetPosition(aSlot->position + TOOLTIPOFFSET);
	anotherSlot->tooltip->SetPosition(anotherSlot->position + TOOLTIPOFFSET);

	if (anotherSlot->icon)
	{
		anotherSlot->icon->SetPosition(anotherSlot->position);
	}
	if (aSlot->icon)
	{
		aSlot->icon->SetPosition(aSlot->position);
	}

	if (!myIsToggled)
	{
		RemoveToolbarFromScene();
		AddToolbarToScene();
	}


	return true;
}

void Inventory::PopulateIconLookup(const std::string& aFilePath)
{
	FiskJSON::Object& root = AssetManager::GetInstance().GetJSON(aFilePath).GetAsJSON();

	std::string prefix = "";
	std::string suffix = "";
	root["GlobalPrefix"].GetIf(prefix);
	root["GlobalSuffix"].GetIf(suffix);
	for (auto& i : root["Bindings"])
	{
		std::string content;
		if (i.second->GetIf(content))
		{
			myIconLookup[ItemIdFromString(i.first)] = prefix + content + suffix;
		}
	}
}

void Inventory::SetToolTip(Slot& aSlot)
{
	if (!IsEmpty(aSlot))
	{
		static std::unordered_map<ItemId, std::wstring> tooltips;

		if (tooltips.empty())
		{
			std::string name = StringFromItemId(aSlot.item.myItemId);

			FiskJSON::Object& file = AssetManager::GetInstance().GetJSON("data/ItemToolTips.json").GetAsJSON();

			std::string content;
			for (auto& i : file["ToolTips"])
			{
				if (i.second->GetIf(content))
				{
					tooltips[ItemIdFromString(i.first)] = std::wstring(content.begin(), content.end());
				}
				}

			}

		if (tooltips.find(aSlot.item.myItemId) == tooltips.end())
		{
#ifndef _RETAIL
			tooltips[aSlot.item.myItemId] = L"AAAAAAAAAAAAAH#Does not have tooltip :c";
#else
			tooltips[aSlot.item.myItemId] = L"#";
#endif // !_RETAIL
		}


		aSlot.tooltip->SetText(tooltips[aSlot.item.myItemId].substr(tooltips[aSlot.item.myItemId].find('#') + 1));
		aSlot.tooltip->SetTitle(tooltips[aSlot.item.myItemId].substr(0, tooltips[aSlot.item.myItemId].find('#')));
		}
	}

