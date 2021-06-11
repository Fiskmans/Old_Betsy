#pragma once

#include "GamlaBettan\Component.h"

class MeshComponent : public Component
{
public:
	MeshComponent(const UseDefaults&);
	MeshComponent(const std::string& aModelname);
	MeshComponent(AssetHandle aModelAsset);
	~MeshComponent();

	void Update(const FrameData& aFrameData, EntityID aEntityID) override;

#if USEIMGUI
	void ImGui(EntityID aEntityID) override;
#endif

private:
	ModelInstance* myInstance;
};

