#include "pch.h"
#include "ComponentManager.h"

#include "GamlaBettan\ComponentSystem.h"

void ComponentManager::RegisterSystem(ComponentSystemBase* aSystem)
{
	mySystems.push_back(aSystem);
	mySystemsByName[aSystem->myName] = aSystem;
}

void ComponentManager::Update(const ComponentBase::FrameData& aFrameData)
{
	for (ComponentSystemBase* system : mySystems)
	{
		system->Update(aFrameData);
	}
}

void ComponentManager::RemoveAllComponents(EntityID aEntityID)
{
	for (ComponentSystemBase* componentSystem : mySystems)
	{
		componentSystem->Return(aEntityID);
	}
}

#if USEIMGUI
void ComponentManager::RunImGuiFor(EntityID aEntityID)
{
	for (ComponentSystemBase* componentSystem : mySystems)
	{
		componentSystem->RunImGuiFor(aEntityID);
	}
}

void ComponentManager::ImGui()
{
	int colums = static_cast<int>(ImGui::GetWindowWidth() / 150.f);
	colums = std::min(colums, static_cast<int>(mySystems.size()));
	if (colums > 0)
	{
		int height = static_cast<int>(((mySystems.size()-1) / colums) + 1);
		ImGui::BeginChild("ComponentManager",ImVec2(0.f, 37.f * height + 7.f), true);
		ImGui::Columns(colums);
		for (size_t i = 0; i < colums; i++)
		{
			for (size_t j = i; j < mySystems.size(); j += colums)
			{
				mySystems[j]->ImGui();
			}
			ImGui::NextColumn();
		}
		ImGui::EndChild();
	}
}

void ComponentManager::Serialize(EntityID aEntityID, FiskJSON::Object& aObject)
{
	for (auto& system : mySystems)
	{
		system->Serialize(aEntityID, aObject);
	}
}

void ComponentManager::Deserialize(EntityID aEntityID, FiskJSON::Object& aObject)
{
	for (auto& keyValuePair : aObject)
	{
		if (mySystemsByName.count(keyValuePair.first) == 0) 
		{
			SYSERROR("No component system registered with that name", keyValuePair.first, std::to_string(aEntityID));
			continue; 
		}
		mySystemsByName[keyValuePair.first]->Deserialize(aEntityID, *keyValuePair.second);
	}
}
#endif