#pragma once
#include "WindowData.h"
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

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;

class CGraphicsEngine
{
public:
	CGraphicsEngine();
	~CGraphicsEngine();

	bool Init(const Window::WindowData& aWindowData,ID3D11Device* aDeviceOverride,ID3D11DeviceContext* aContextOverride);
	void BeginFrame(float aClearColor[4]);
	void RenderFrame(Scene* aScene);
	void RenderMovie(const std::vector<SpriteInstance*>& aSpriteList);
	void RenderSprites(const std::vector<SpriteInstance*>& aSpriteList, const bool aShouldRenderExtraSprites = false);
	void RenderText(const std::vector<TextInstance*>& aTextList);
	void EndFrame();

	void AddExtraSpriteToRender(SpriteInstance* aSprite);

	SpriteFactory& GetSpriteFactory();
	WindowHandler* GetWindowHandler();
	DirectX11Framework* GetFrameWork();
	LightLoader* GetLightLoader();
	class SpriteRenderer* GetSpriteRenderer();
	class RenderManager* GetRendreManarger();

	void SubscribeToMessages();

	void UnsubscribeToMessages();

#if USEIMGUI
	void Imgui();
#endif // !_RETAIL


private:
	class RenderManager* myRendreManarger;

	DirectX11Framework* myFrameWork;
	WindowHandler* myWindowHandler;
	SpriteFactory mySpriteFactory;
	LightLoader* myLightLoader;
};