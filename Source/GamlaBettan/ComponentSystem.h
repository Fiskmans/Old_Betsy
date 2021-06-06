#pragma once
#include "GamlaBettan\Component.h"
#include "GamlaBettan\ComponentManager.h"

#include "CommonUtilities\ObjectPool.h"
#include "CommonUtilities\Singleton.hpp"

class Entity;

class ComponentSystemBase
{
public:
	ComponentSystemBase() = default;
	virtual ~ComponentSystemBase() = default;

	virtual void Update(const Component::FrameData& aFrameData) = 0;
};

template<class T>
concept DerivedFromComponent = requires() { std::derived_from<T, Component>; };

template<DerivedFromComponent ComponentType>
class ComponentSystem
	: public ComponentSystemBase, public CommonUtilities::Singleton<ComponentSystem<ComponentType>>
{
public:
	ComponentSystem();

	struct ComponentNode
	{
		ComponentType myComponent;
		Entity* myEntity;
	};
	template<class... Args>
	ComponentType* Get(Entity* aEntity,Args... args);
	void Return(ComponentType* aComponent);

	void Update(const Component::FrameData& aFrameData) override;

private:
	CommonUtilities::ObjectPool<ComponentNode> myPool;
};

template<DerivedFromComponent ComponentType>
inline ComponentSystem<ComponentType>::ComponentSystem()
{
	ComponentManager::GetInstance().RegisterSystem(this);
}


template<DerivedFromComponent ComponentType>
template<class... Args>
inline ComponentType* ComponentSystem<ComponentType>::Get(Entity* aEntity, Args... args)
{
	ComponentNode* node = myPool.Get(args...);
	node->myEntity = aEntity;
	return &node->myComponent;
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::Return(ComponentType* aComponent)
{
	myPool.Return(reinterpret_cast<ComponentNode*>(static_cast<unsigned char*>(aComponent) - offsetof(ComponentNode, myComponent)));
}

template<DerivedFromComponent ComponentType>
inline void ComponentSystem<ComponentType>::Update(const Component::FrameData& aFrameData)
{
	for (ComponentNode* i : myPool)
	{
		i->myComponent->Update(aFrameData, i->myEntity);
	}
}
