#pragma once
#include "GamlaBettan\ComponentSystem.h"

class Entity
{
public:
	M44f myTransform;

	template<DerivedFromComponent ComponentType, class... Args>
	ComponentType* AddComponent(Args... args);

};

template<DerivedFromComponent ComponentType, class ...Args>
inline ComponentType* Entity::AddComponent(Args ...args)
{
	return ComponentSystem<ComponentType>::Get(this, args...);
}
