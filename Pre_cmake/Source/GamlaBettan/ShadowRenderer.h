#pragma once

class DirectX11Framework;
class PixelShader;
class ShadowRenderer
{
public:
	bool Init(DirectX11Framework* aFramework);

	const std::array<Camera*, 6>& GetCameras();
	void BindshadowsToSlots(int aSlot);
	void Render(PointLight* aLight,Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping);

private:
	struct ID3D11Buffer* myFrameBuffer = nullptr;
	struct ID3D11Buffer* myObjectBuffer = nullptr;
	ID3D11DeviceContext* myContext;
	PixelShader* myShader;
	std::array<Camera*,6> myCameras = { nullptr };
	ID3D11DepthStencilView* myDepth = { nullptr };
	ID3D11ShaderResourceView* myDepthsResource = { nullptr };
	ID3D11RenderTargetView* myRenderTarget = { nullptr };
};

