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
