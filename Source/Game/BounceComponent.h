#pragma once
#include "GamlaBettan\Component.h"

class BounceComponent : public Component
{
public:
	BounceComponent();
	BounceComponent(Component::UseDefaults&);

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) override;

#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) override;
#endif

	float mySpeed = 1;
};

