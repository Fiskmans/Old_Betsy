#include "pch.h"

#include "ParentComponent.h"
#include "EntityManager.h"

ParentComponent::ParentComponent()
{
}

ParentComponent::ParentComponent(const FiskJSON::Object& aObject)
{
	myChildren = EntityManager::GetInstance().LoadCollection(aObject["children"]);
}

bool ParentComponent::RemoveChild(EntityID aEntity)
{
	decltype(myChildren)::iterator it = std::find(std::begin(myChildren), std::end(myChildren), aEntity);
	if (it == std::end(myChildren)) { return false; }

	myChildren.erase(it);
	return true;
}

void ParentComponent::Update(const FrameData&, EntityID)
{
}

void ParentComponent::ImGui(EntityID)
{
	for (EntityID child : myChildren)
	{
		if (ImGui::TreeNode(std::to_string(child).c_str()))
		{
			ComponentManager::GetInstance().RunImGuiFor(child);
			ImGui::TreePop();
		}
	}
}

void ParentComponent::Serialize(FiskJSON::Object& aObject)
{
	FiskJSON::Object* children = new FiskJSON::Object();
	children->MakeArray();
	EntityManager::GetInstance().SaveCollection(myChildren, children->Get<FiskJSON::ArrayWrapper>());
	aObject.AddChild("children", children);
}
