#pragma once
#include <vector>
#include "PointLight.h"
#include "FullscreenRenderer.h"
#include <Observer.hpp>
#include <unordered_map>
#include "SpotLight.h"
#include "GBuffer.h"
#include "ShaderFlags.h"

class Scene;
class RenderStateManager;
class FullscreenTexture;
class DepthRenderer;
class Texture;

class DeferredRenderer : public Observer
{
public:
	DeferredRenderer() = default;
	~DeferredRenderer() = default;

	bool Init(class DirectX11Framework* aFramework,struct Texture** aPerlinPointer, DepthRenderer* aShadowRenderer);

	std::vector<class ModelInstance*> GenerateGBuffer(class Camera* aCamera, std::vector<class ModelInstance*>& aModelList, std::unordered_map<ModelInstance*, short>& aBoneMapping, FullscreenTexture* aBacksideTexture, RenderStateManager* aRenderStateManager, std::vector<class Decal*>& aDecals, GBuffer* aGBuffer,GBuffer* aBufferGBuffer, FullscreenRenderer& aFullscreenRenderer, Scene* aScene,FullscreenTexture* aDepth,BoneTextureCPUBuffer& aBoneTextureBuffer);

	void Render(FullscreenRenderer& aFullscreenRenderer, std::vector<PointLight*>& aPointLightList, std::vector<SpotLight*>& aSpotLightList, Scene* aScene, RenderStateManager* aRenderStateManager, std::unordered_map<ModelInstance*, short>& aBoneMapping);

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;

	void SubscribeToMessages();
	void UnsubscribeToMessages();

	void MapEnvLightBuffer(Scene* aScene);
private:
	PixelShader* GetPixelShader(size_t flags);

	//Secrets
	bool myIsDiscoEnabled = false;
	bool myIsInAdamMode = false;
	Texture* myAdamTexture = nullptr;
	DepthRenderer* myShadowRenderer;

	GAMEMETRIC(float, myCloudIntensity, CLOUDINTENSITY, 0.3f);

	long long myCreateTime = 0;
	struct ID3D11DeviceContext* myContext = nullptr;
	struct ID3D11Device* myDevice = nullptr;
	std::map<size_t, PixelShader*> myPixelShaders;
	class PixelShader* myBackFaceShader = nullptr;
	ID3D11Buffer* myFrameBuffer = nullptr;
	ID3D11Buffer* myObjectBuffer = nullptr;
	ID3D11Buffer* myPixelEnvLightBuffer = nullptr;
	ID3D11Buffer* myPixelPointLightBuffer = nullptr;
	ID3D11Buffer* myPixelSpotLightBuffer = nullptr;
	ID3D11Buffer* myDecalBuffer = nullptr;
	Texture** myPerlinPointer = nullptr;
};