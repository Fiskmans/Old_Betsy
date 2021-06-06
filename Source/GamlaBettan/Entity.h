#pragma once
#include "GamlaBettan\ComponentSystem.h"

class Entity
{
public:
	M44f myTransform;

	template<DerivedFromComponent ComponentType, class... Args>
	ComponentType* AddComponent(Args... args);

	V3F GetPosition();
};

template<DerivedFromComponent ComponentType, class ...Args>
inline ComponentType* Entity::AddComponent(Args ...args)
{
	return ComponentSystem<ComponentType>::Get(this, args...);
}

inline V3F Entity::GetPosition()
{
	return myTransform.Row(3);
}