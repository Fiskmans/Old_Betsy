#pragma once
#include "GamlaBettan\Component.h"
class FreeCam : public Component
{
public:

	FreeCam();
	FreeCam(const UseDefaults&);

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) override;
#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) override;
#endif

private:
	float myRotationSpeed = 1;
	float myMovementSpeed = 50;
};

