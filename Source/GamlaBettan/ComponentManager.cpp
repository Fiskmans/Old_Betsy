#include "pch.h"
#include "ComponentManager.h"

#include "GamlaBettan\ComponentSystem.h"

void ComponentManager::RegisterSystem(ComponentSystemBase* aSystem)
{
	mySystems.push_back(aSystem);
}

void ComponentManager::Update(const Component::FrameData& aFrameData)
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

void ComponentManager::RunImGuiFor(EntityID aEntityID)
{
	for (ComponentSystemBase* componentSystem : mySystems)
	{
		componentSystem->RunImGuiFor(aEntityID);
	}
}

void ComponentManager::ImGui()
{
	size_t colums = ImGui::GetWindowWidth() / 150;
	colums = MIN(colums, mySystems.size());
	if (colums > 0)
	{
		size_t height = ((mySystems.size()-1) / colums) + 1;
		ImGui::BeginChild("ComponentManager",ImVec2(0,37 * height + 7), true);
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
