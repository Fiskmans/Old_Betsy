#pragma once
#include "GamlaBettan\EntityID.h"
#include "GamlaBettan\ComponentSystem.h"

class Entity
{
public:

	M44f myTransform;

	template<DerivedFromComponent ComponentType, class... Args>
	ComponentType* AddComponent(Args... args);

#if USEIMGUI
	std::string myEditorName = "UNAMED";
	void Imgui();
#endif

	V3F GetPosition();

private:
	friend class EntityManager;

	EntityID myID;

	void Construct();
	void Destruct();
};

template<DerivedFromComponent ComponentType, class ...Args>
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

inline void Entity::Construct()
{
	myTransform = M44f::Identity();
}

inline void Entity::Destruct()
{
}
