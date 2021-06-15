#pragma once
#include "PointLight.h"
#include "FullscreenRenderer.h"
#include <Observer.hpp>
#include "SpotLight.h"
#include "GBuffer.h"
#include "ShaderFlags.h"
#include "GamlaBettan\Decal.h"

class Scene;
class RenderStateManager;
class FullscreenTexture;
class DepthRenderer;
class Texture;

class DeferredRenderer
{
public:
	DeferredRenderer() = default;
	~DeferredRenderer() = default;

	bool Init(DirectX11Framework* aFramework,AssetHandle aPerlinHandle, DepthRenderer* aShadowRenderer);

	std::vector<ModelInstance*> GenerateGBuffer(Camera* aCamera, std::vector<ModelInstance*>& aModelList, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping, FullscreenTexture* aBacksideTexture, RenderStateManager* aRenderStateManager, std::vector<Decal*>& aDecals, GBuffer* aGBuffer,GBuffer* aBufferGBuffer, FullscreenRenderer& aFullscreenRenderer,FullscreenTexture* aDepth,BoneTextureCPUBuffer& aBoneTextureBuffer);

	void Render(FullscreenRenderer& aFullscreenRenderer, std::vector<PointLight*>& aPointLightList, std::vector<SpotLight*>& aSpotLightList, RenderStateManager* aRenderStateManager, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping);

	void MapEnvLightBuffer();
private:
	bool OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize);

	DepthRenderer* myShadowRenderer = nullptr;

	GAMEMETRIC(float, myCloudIntensity, CLOUDINTENSITY, 0.3f);

	long long myCreateTime = 0;
	struct ID3D11DeviceContext* myContext = nullptr;
	struct ID3D11Device* myDevice = nullptr;
	AssetHandle myBackFaceShader;
	ID3D11Buffer* myFrameBuffer = nullptr;
	ID3D11Buffer* myObjectBuffer = nullptr;
	ID3D11Buffer* myPixelEnvLightBuffer = nullptr;
	ID3D11Buffer* myPixelPointLightBuffer = nullptr;
	ID3D11Buffer* myPixelSpotLightBuffer = nullptr;
	ID3D11Buffer* myDecalBuffer = nullptr;
	AssetHandle myPerlinHandle;
};