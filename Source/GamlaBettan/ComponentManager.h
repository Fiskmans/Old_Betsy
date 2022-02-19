#pragma once

#include "GamlaBettan\EntityID.h"
#include "CommonUtilities\Singleton.hpp"

#include "GamlaBettan\ComponentBase.h"

class ComponentSystemBase;

class ComponentManager : public CommonUtilities::Singleton<ComponentManager>
{
public:

	void RegisterSystem(ComponentSystemBase* aSystem);

	void Update(const ComponentBase::FrameData& aFrameData);

	void RemoveAllComponents(EntityID aEntityID);

#if USEIMGUI
	void RunImGuiFor(EntityID aEntityID);
	void ImGui();
	void Serialize(EntityID aEntityID, FiskJSON::Object& aObject);
	void Deserialize(EntityID aEntityID, FiskJSON::Object& aObject);
#endif

private:
	std::vector<ComponentSystemBase*> mySystems;
	std::unordered_map<std::string, ComponentSystemBase*> mySystemsByName;
};

