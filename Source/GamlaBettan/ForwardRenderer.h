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
class Skybox;
struct Texture;
class Scene;
class PointLight;
struct D3D11_MAPPED_SUBRESOURCE;
class DepthRenderer;

class ForwardRenderer : public Observer
{
public:
	~ForwardRenderer();

	bool Init(DirectX11Framework* aFramework, const std::string& aThroughWallPSName, const std::string& aEnemyThroughWallPSName, Texture** aPerlinPointer,DepthRenderer* aDepthRenderer);

	void Render(std::vector<ModelInstance*>& aModelList, Camera* aCamera, Scene* aScene, std::vector<std::array<PointLight*, NUMBEROFPOINTLIGHTS>>& aLightList, std::unordered_map<ModelInstance*, short>& aBoneMapping, class RenderStateManager& aStateManager,BoneTextureCPUBuffer& aBoneBuffer);

	void SetSkyboxTexture(Texture* aTexture);
	void SetSkybox(Skybox* aSkyBox);

	void SubscribeToMessages();
	void UnsubscribeToMessages();

private:
	GAMEMETRIC(float, myCloudIntensity, CLOUDINTENSITY, 0.3f);

	bool myIsInAdamMode = false;
	Texture* myAdamTexture = nullptr;

	inline void RenderModel(ModelInstance* aModelInstance, std::array<PointLight*, NUMBEROFPOINTLIGHTS>* aLightList, std::unordered_map<ModelInstance*, short>& aBoneMapping, D3D11_MAPPED_SUBRESOURCE& aBuffer, const Camera* aCamera, BoneTextureCPUBuffer& aBoneBuffer);
	bool ReloadPixelShader(const std::string& aFilePath);

	ID3D11DeviceContext* myContext = nullptr;
	ID3D11Device* myDevice = nullptr;
	ID3D11Buffer* myFrameBuffer = nullptr;
	ID3D11Buffer* myObjectBuffer = nullptr;
	Skybox* myskybox = nullptr;
	Texture* mySkyboxTexture = nullptr;
	DepthRenderer* myDepthRender = nullptr;

	class PixelShader* myThroughWallShader = nullptr;
	class PixelShader* myEnemyThroughWallShader = nullptr;
	Tools::FileWatcher::UniqueID myPSFileHandle;
	Tools::FileWatcher::UniqueID myEnemyPSFileHandle;
	Texture** myPerlinPointer;

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
};

