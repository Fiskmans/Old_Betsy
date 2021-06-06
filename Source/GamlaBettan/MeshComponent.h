#pragma once

#include "GamlaBettan\Component.h"

class MeshComponent : public Component
{
public:
	MeshComponent(const std::string& aModelname);
	~MeshComponent();

	// Inherited via Component
	virtual void Update(const FrameData& aFrameData, Entity* aEntity) override;

private:
	ModelInstance* myInstance;
};

