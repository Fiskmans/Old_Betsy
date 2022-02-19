#pragma once
#include "GamlaBettan\EntityID.h"
#include "GamlaBettan\ComponentSystem.h"

class Entity
{
public:

	M44f myTransform;

	template<DerivedFromComponent ComponentType, typename... Args>
	ComponentType* AddComponent(Args... args);

#if USEIMGUI
	std::string myEditorName = "Unnamed entity";
	void Imgui();
	void Serialize(FiskJSON::Object& aObject) const;
	void Deserialize(const FiskJSON::Object& aObject);
#endif

	V3F GetPosition();

private:
	friend class EntityManager;

	EntityID myID = static_cast<EntityID>(-1);
	bool myIsActive : 1 = false;

	void Activate();
	void Deactivate();
};

template<DerivedFromComponent ComponentType, typename ...Args>
inline ComponentType* Entity::AddComponent(Args ...args)
{
	return ComponentSystem<ComponentType>::GetInstance().Get(myID, args...);
}

#if USEIMGUI
inline void Entity::Imgui()
{
	ImGui::Text(myEditorName.c_str());
	ImGui::PushID(this);


	ImGui::PopID();
}
#endif

inline V3F Entity::GetPosition()
{
	return myTransform.Row(3);
}

inline void Entity::Activate()
{
	myTransform = M44f::Identity();
	myIsActive = true;
}

inline void Entity::Deactivate()
{
	myIsActive = false;
}
