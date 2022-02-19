#pragma once
#include "ComponentBase.h"
#include "ComponentSystem.h"

template<class ComponentType> 
class AutoRegisterComponentSystem
{
public:
	AutoRegisterComponentSystem() { ComponentSystem<ComponentType>::GetInstance(); }
};

template<class ParentComponentType>
class Component : public ComponentBase
{
public:
	template<typename... Args>
	Component(Args&&... args) : ComponentBase(std::forward<Args>(args)...) {}

private:
	inline static AutoRegisterComponentSystem<ParentComponentType> autoRegister = {};
};

