#pragma once
#include "GamlaBettan\EntityID.h"
#include "GamlaBettan\ComponentBase.h"
#include "GamlaBettan\ComponentManager.h"

#include "CommonUtilities\ObjectPool.h"
#include "CommonUtilities\Singleton.hpp"

class Entity;

class ComponentSystemBase
{
public:

	std::string myName;

	ComponentSystemBase() = default;
	virtual ~ComponentSystemBase() = default;

	virtual void Update(const ComponentBase::FrameData& aFrameData) = 0;
	virtual void Return(EntityID aEntityID) = 0;
#if USEIMGUI
	virtual void RunImGuiFor(EntityID aEntityID) = 0;
	virtual void ImGui() = 0;
	virtual void AddDefaultComponent(EntityID aEntityID) = 0;
	virtual void Serialize(EntityID aEntityID, FiskJSON::Object& aJSONToSerializeTo) = 0;
	virtual void Deserialize(EntityID aEntityID, FiskJSON::Object& aJSONToSerializeFrom) = 0;
#endif
};

template<class T>
concept DerivedFromComponent = 
	requires() 
	{ 
		std::derived_from<T, ComponentBase>; 
	}
	&& requires() 
	{ 
		T(std::declval<FiskJSON::Object>()); 
	};

template<DerivedFromComponent ComponentType>
class ComponentSystem
	: public ComponentSystemBase, public CommonUtilities::Singleton<ComponentSystem<ComponentType>>
{
public:
	ComponentSystem();

	class ComponentNode
	{
	public:
		template<class... Args>
		ComponentNode(Args&&... args) : myComponent(args...), myEntityID(0) { }

	private:
		friend ComponentSystem;
		ComponentType myComponent;
		EntityID myEntityID;
	};

	template<class... Args>
	ComponentType* Get(EntityID aEntityID, Args&&... args);
	[[nodiscard]] ComponentType* Retreive(EntityID aEntityID);
	void Return(EntityID aEntityID) override;

	void Update(const ComponentBase::FrameData& aFrameData) override;

#if USEIMGUI
	void RunImGuiFor(EntityID aEntityID) override;
	void ImGui() override;
	void AddDefaultComponent(EntityID aEntityID) override;
	void Serialize(EntityID aEntityID, FiskJSON::Object& aJSONToSerializeTo) override;
	void Deserialize(EntityID aEntityID, FiskJSON::Object& aJSONToSerializeFrom) override;
#endif

private:

	std::unordered_map<EntityID,ComponentNode*> myMappings;
	CommonUtilities::ObjectPool<ComponentNode> myPool;
};

template<DerivedFromComponent ComponentType>
inline ComponentSystem<ComponentType>::ComponentSystem()
{
	myName = type_traits::Name<ComponentType>();
	ComponentManager::GetInstance().RegisterSystem(this);
}


template<DerivedFromComponent ComponentType>
template<class... Args>
inline ComponentType* ComponentSystem<ComponentType>::Get(EntityID aEntityID, Args&&... args)
{
	if (myMappings.count(aEntityID) != 0)
	{
		SYSERROR("Adding more than 1 component of the same type", myName);
		return nullptr;
	}
	ComponentNode* node = myPool.Get(std::forward<Args>(args)...);
	node->myEntityID = aEntityID;
	myMappings[aEntityID] = node;
	return &node->myComponent;
}

template<DerivedFromComponent ComponentType>
inline ComponentType* ComponentSystem<ComponentType>::Retreive(EntityID aEntityID)
{
	if (myMappings.count(aEntityID) == 0)
	{
		return nullptr;
	}
	return &myMappings[aEntityID]->myComponent;
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::Return(EntityID aEntityID)
{
	if (myMappings.count(aEntityID) == 0)
	{
		return;
	}
	myPool.Return(myMappings[aEntityID]);
	myMappings.erase(aEntityID);
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::Update(const ComponentBase::FrameData& aFrameData)
{
	for (ComponentNode* i : myPool)
	{
		i->myComponent.Update(aFrameData, i->myEntityID);
	}
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::RunImGuiFor(EntityID aEntityID)
{
	ComponentType* comp = Retreive(aEntityID);
	if (comp)
	{
		bool open = ImGui::TreeNode(myName.c_str());
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("EntityID", &aEntityID, sizeof(aEntityID));
			ImGui::EndDragDropSource();
		}
		if (open)
		{
			comp->ImGui(aEntityID);
			ImGui::TreePop();
		}
	}
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::ImGui()
{
	ImGui::Button(myName.c_str(), ImVec2(150, 30));
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityID");
		if (payload)
		{
			Return(*static_cast<EntityID*>(payload->Data));
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::BeginDragDropSource())
	{
		ImGui::Text(myName.c_str());

		ComponentSystemBase* payload = this;
		ImGui::SetDragDropPayload("ComponentAdd", &payload, sizeof(payload));

		ImGui::EndDragDropSource();
	}
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::AddDefaultComponent(EntityID aEntityID)
{
	Get(aEntityID, ComponentBase::UseDefaults());
}


template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::Serialize(EntityID aEntityID, FiskJSON::Object& aJSONToSerializeTo)
{
	ComponentType* component = Retreive(aEntityID);
	if (component)
	{
		FiskJSON::Object* componentJSON = new FiskJSON::Object();
		component->Serialize(*componentJSON);
		aJSONToSerializeTo.AddChild(type_traits::Name<ComponentType>(), componentJSON);
	}
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::Deserialize(EntityID aEntityID, FiskJSON::Object& aJSONToSerializeFrom)
{
	Get(aEntityID, aJSONToSerializeFrom);
}