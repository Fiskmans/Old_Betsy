#pragma once

#include "GamlaBettan\EntityID.h"
#include "CommonUtilities\Singleton.hpp"

#include "GamlaBettan\Component.h"

class ComponentSystemBase;

class ComponentManager : public CommonUtilities::Singleton<ComponentManager>
{
public:

	void RegisterSystem(ComponentSystemBase* aSystem);

	void Update(const Component::FrameData& aFrameData);

	void RemoveAllComponents(EntityID aEntityID);

#if USEIMGUI
	void RunImGuiFor(EntityID aEntityID);
	void ImGui();
#endif

private:
	std::vector<ComponentSystemBase*> mySystems;
};

