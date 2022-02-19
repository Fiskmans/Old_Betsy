#pragma once
#include "GamlaBettan\Component.h"

class BounceComponent : public Component<BounceComponent>
{
public:
	BounceComponent();
	BounceComponent(const FiskJSON::Object& aObject);
	BounceComponent(Component::UseDefaults&);

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) override;

#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) override;
	virtual void Serialize(FiskJSON::Object& aObject) override;
#endif

	float mySpeed = 1;
};

