#pragma once

#include "CommonUtilities\Singleton.hpp"

#include "GamlaBettan\Component.h"

class ComponentSystemBase;

class ComponentManager : public CommonUtilities::Singleton<ComponentManager>
{
public:

	void RegisterSystem(ComponentSystemBase* aSystem);

	void Update(const Component::FrameData& aFrameData);

private:
	std::vector<ComponentSystemBase*> mySystems;
};

