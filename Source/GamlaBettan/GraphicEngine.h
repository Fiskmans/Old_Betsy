#pragma once
#include "ModelLoader.h"
#include "SpriteFactory.h"

class DirectX11Framework;
class ForwardRenderer;
class LightLoader;
class WindowHandler;
class SpriteRenderer;
class Scene;
class Camera;
class Skybox;
class TextInstance;
class RenderManager;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;

class CGraphicsEngine
{
public:
	CGraphicsEngine();
	~CGraphicsEngine();

	bool Init();
	bool Init(V2ui aWindowSize);
	void BeginFrame(float aClearColor[4]);
	void RenderFrame(Scene* aScene);
	void RenderMovie(const std::vector<SpriteInstance*>& aSpriteList);
	void RenderSprites(const std::vector<SpriteInstance*>& aSpriteList, const bool aShouldRenderExtraSprites = false);
	void RenderText(const std::vector<TextInstance*>& aTextList);
	void EndFrame();

	void AddExtraSpriteToRender(SpriteInstance* aSprite);

	SpriteFactory& GetSpriteFactory();
	DirectX11Framework* GetFrameWork();
	LightLoader* GetLightLoader();
	SpriteRenderer* GetSpriteRenderer();
	RenderManager* GetRendreManarger();

#if USEIMGUI
	void Imgui();
#endif // !_RETAIL


private:
	bool InitInternal();

	RenderManager* myRendreManarger;

	DirectX11Framework* myFrameWork;
	SpriteFactory mySpriteFactory;
	LightLoader* myLightLoader;
};