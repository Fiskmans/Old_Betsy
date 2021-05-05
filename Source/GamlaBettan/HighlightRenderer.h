#pragma once
#include <vector>

class HighlightRenderer
{
public:
	HighlightRenderer() = default;
	~HighlightRenderer() = default;

	bool Init(class DirectX11Framework* aFramework);

	void Render(const std::vector<class ModelInstance*>& aModels,class Camera* aCamera, std::unordered_map<ModelInstance*, short>& aBoneMapping);

private:
	long long myCreateTime = 0;
	struct ID3D11DeviceContext* myContext = nullptr;
	class PixelShader* myPixelShader = nullptr;
	struct ID3D11Buffer* myFrameBuffer = nullptr;
	struct ID3D11Buffer* myObjectBuffer = nullptr;

};

