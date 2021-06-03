#pragma once

class DirectX11Framework;
class PixelShader;
class SpotLight;
class PointLight;
class EnvironmentLight;
struct Decal;

class DepthRenderer
{
public:
	~DepthRenderer();

	bool Init(DirectX11Framework* aFramework);

	const std::array<Camera*, 6>& GetCameras();
	const Camera* GetEnvirontmentCamera();
	void BindshadowsToSlots(int aSlot);
	void BindShadowDensityToSlot(int aSlot);
	void Render(PointLight* aLight,Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping);
	void RenderSpotLightDepth(SpotLight* aSpotlight, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping);
	void RenderEnvironmentDepth(EnvironmentLight* aLight, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping, std::function<void()> aPreRenderFunction = []() {});
	ID3D11ShaderResourceView* RenderDecalDepth(Decal* aSpotlight, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping);

private:
	
	ID3D11ShaderResourceView* myLastDepthView = { nullptr };

	void Render(Camera* aCamera, const std::vector<ModelInstance*>& aModelList, std::unordered_map<ModelInstance*, short>& aBoneMapping);
	struct ID3D11Buffer* myFrameBuffer = nullptr;
	struct ID3D11Buffer* myObjectBuffer = nullptr;
	ID3D11DeviceContext* myContext;
	AssetHandle myShader;
	ID3D11Device* myDevice;
	std::array<Camera*,6> myCameras = { nullptr };
	Camera* myEnvironmentCamera = { nullptr };

	// 1x6
	ID3D11DepthStencilView* myDepth1x6 = { nullptr };
	ID3D11ShaderResourceView* myDepthsResource1x6 = { nullptr };
	ID3D11RenderTargetView* myRenderTarget1x6 = { nullptr };

	// 1x1
	ID3D11DepthStencilView* myDepth1x1 = { nullptr };
	ID3D11ShaderResourceView* myDepthsResource1x1 = { nullptr };
	ID3D11RenderTargetView* myRenderTarget1x1 = { nullptr };
	ID3D11ShaderResourceView* myRenderTargetResourceView1x1 = { nullptr };

	ID3D11RenderTargetView* myDecalRenderTarget = { nullptr };
};

