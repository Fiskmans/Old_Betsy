#pragma once
#include "GamlaBettan\Component.h"
class FreeCam : public Component<FreeCam>
{
public:

	FreeCam();
	FreeCam(const FiskJSON::Object& aObject);
	FreeCam(const UseDefaults&);

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) override;
#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) override;
	virtual void Serialize(FiskJSON::Object& aObject) override;
#endif

private:
	float myRotationSpeed = 1;
	float myMovementSpeed = 50;
};

