#pragma once
#include "Asset.h"

class HighlightRenderer
{
public:
	HighlightRenderer() = default;
	~HighlightRenderer() = default;

	bool Init(class DirectX11Framework* aFramework);

	void Render(const std::vector<ModelInstance*>& aModels, Camera* aCamera, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping);

private:
	long long myCreateTime = 0;
	struct ID3D11DeviceContext* myContext = nullptr;
	AssetHandle myPixelShader;
	struct ID3D11Buffer* myFrameBuffer = nullptr;
	struct ID3D11Buffer* myObjectBuffer = nullptr;

};

