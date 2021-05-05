#include "pch.h"
#include "pch.h"
#include "Item.h"

bool Item::UseCharge()
{
	if (myCharges > 0)
	{
		--myCharges;
		return true;
	}
	return false;
}

bool Item::Consume()
{
	if (myAmount > 0)
	{
		--myAmount;
		return true;
	}
	return false;
}

void ItemImgui()
{
#if USEIMGUI
	ImGui::Text("Loaded Items");
	ImGui::Separator();
	for (auto& i : ItemIdToStringMap())
	{
		ImGui::PushID(i.second.c_str());
		if (ImGui::Button("Give"))
		{
			ItemCreationStruct item;
			item.myItemId = i.first;
			Message msg;
			msg.myData = &item;
			msg.myMessageType = MessageType::SpawnItem;
			PostMaster::GetInstance()->SendMessages(msg);
		}
		ImGui::SameLine();
		ImGui::Text("%s: %zu", i.second.c_str(), i.first);
		ImGui::PopID();
	}
#endif
}