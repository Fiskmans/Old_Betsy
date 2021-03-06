#pragma once
#include <vector>
#include <array>
#include "Observer.hpp"
#include <unordered_map>
#include "FileWatcher.h"
class DirectX11Framework;
class ModelInstance;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
class Camera;
class Texture;
class RenderScene;
class PointLight;
struct D3D11_MAPPED_SUBRESOURCE;
class DepthRenderer;

class ForwardRenderer
{
public:
	~ForwardRenderer();

	bool Init(DirectX11Framework* aFramework, AssetHandle aPerlinHandle, DepthRenderer* aDepthRenderer);

	void Render(std::vector<ModelInstance*>& aModelList, Camera* aCamera, std::vector<std::array<PointLight*, NUMBEROFPOINTLIGHTS>>& aLightList, std::unordered_map<ModelInstance*, short>& aBoneMapping, class RenderStateManager& aStateManager,BoneTextureCPUBuffer& aBoneBuffer);

	void SetSkyboxTexture(AssetHandle aTexture);
	void SetSkybox(ModelInstance* aSkyBox);


private:
	GAMEMETRIC(float, myCloudIntensity, CLOUDINTENSITY, 0.3f);

	inline void RenderModel(
		ModelInstance* aModelInstance, std::array<PointLight*, NUMBEROFPOINTLIGHTS>* aLightList, std::unordered_map<ModelInstance*, short>& aBoneMapping, 
		D3D11_MAPPED_SUBRESOURCE& aBuffer, BoneTextureCPUBuffer& aBoneBuffer);

	ID3D11DeviceContext* myContext = nullptr;
	ID3D11Device* myDevice = nullptr;
	ID3D11Buffer* myFrameBuffer = nullptr;
	ID3D11Buffer* myObjectBuffer = nullptr;
	ModelInstance* myskybox = nullptr;
	AssetHandle mySkyboxTexture;
	DepthRenderer* myDepthRender = nullptr;

	AssetHandle myPerlinHandle;
};

