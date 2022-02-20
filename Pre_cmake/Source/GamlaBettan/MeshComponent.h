#pragma once

#include "GamlaBettan\Component.h"

class MeshComponent : public Component<MeshComponent>
{
public:
	MeshComponent(const UseDefaults&);
	MeshComponent(const FiskJSON::Object& aObject);
	MeshComponent(const std::string& aModelname);
	MeshComponent(AssetHandle aModelAsset);
	~MeshComponent();

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) override;

#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) override;
	virtual void Serialize(FiskJSON::Object& aObject) override;
#endif

private:
	ModelInstance* myInstance;
};

