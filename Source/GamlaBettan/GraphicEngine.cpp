#include "pch.h"
#include "pch.h"

#include "GraphicEngine.h"
#include "Scene.h"
#include "Skybox.h"
#include "DirectX11Framework.h"
#include "ForwardRenderer.h"
#include "SpriteRenderer.h"
#include "LightLoader.h"
#include "WindowHandler.h"
#include "PointLight.h"
#include <array>
#include "RenderManager.h"
#include <d3d11.h>
#include <CameraFactory.h>

#include "DebugTools.h"

CGraphicsEngine::CGraphicsEngine()
{
	myFrameWork = new DirectX11Framework();
	myWindowHandler = new WindowHandler();
	myRendreManarger = new RenderManager();
	myLightLoader = new LightLoader();
}

CGraphicsEngine::~CGraphicsEngine()
{
	myRendreManarger->Release();

	delete myFrameWork;
	myFrameWork = nullptr;
	delete myWindowHandler;
	myWindowHandler = nullptr;
	SAFE_DELETE(myRendreManarger);
	SAFE_DELETE(myLightLoader);
}

bool CGraphicsEngine::Init(const Window::WindowData& aWindowData, ID3D11Device* aDeviceOverride, ID3D11DeviceContext* aContextOverride)
{
	if (aDeviceOverride && aContextOverride)
	{
		if (!myWindowHandler->Init(aWindowData, nullptr))
		{
			SYSCRASH("Could not init fake windowhandler :c");
			return false;
		}
		if (!myFrameWork->Init(aDeviceOverride,aContextOverride))
		{
			SYSCRASH("Could not init graphics framework :c");
			return false;
		}
	}
	else
	{
		if (!myWindowHandler->Init(aWindowData, myFrameWork))
		{
			SYSCRASH("Could not init window :c");
			return false;
		}
		if (!myFrameWork->Init(myWindowHandler->GetWindowHandle()))
		{
			SYSCRASH("Could not init graphics framework :c");
			return false;
		}
	}

	CCameraFactory::Init(myWindowHandler, NEARPLANE, FARPLANE);
	if (!myModelLoader.Init(myFrameWork))
	{
		SYSCRASH("Cound not init model loader :c");
		return false;
	}
#ifdef _DEBUG
	DebugTools::myModelLoader = &myModelLoader;
#endif // _DEBUG



	if (!mySpriteFactory.Init(myFrameWork))
	{
		SYSCRASH("Cound not init SpriteFactory :c")
			return false;
	}
	if (!myLightLoader->Init(myFrameWork->GetDevice()))
	{
		SYSCRASH("Cound not init light loader :c")
			return false;
	}
	if (!myRendreManarger->Init(myFrameWork, myWindowHandler))
	{
		SYSCRASH("Could not init rendremanargre :c");
		return false;
	}

	return true;
}

void CGraphicsEngine::BeginFrame(float aClearColor[4])
{
	myRendreManarger->BeginFrame(aClearColor);
}

void CGraphicsEngine::RenderFrame(Scene* aScene)
{
	myRendreManarger->Render(aScene);
}

void CGraphicsEngine::RenderMovie(const std::vector<SpriteInstance*>& aSpriteList)
{
	myRendreManarger->RenderMovie(aSpriteList);
}

void CGraphicsEngine::RenderSprites(const std::vector<SpriteInstance*>& aSpriteList, const bool aShouldRenderExtraSprites)
{
	myRendreManarger->RenderSprites(aSpriteList, aShouldRenderExtraSprites);
}

void CGraphicsEngine::RenderText(const std::vector<TextInstance*>& aTextList)
{
	myRendreManarger->RenderText(aTextList);
}

void CGraphicsEngine::EndFrame()
{
	myRendreManarger->EndFrame();
	myFrameWork->EndFrame();
	myModelLoader.FlushChanges();
}

void CGraphicsEngine::AddExtraSpriteToRender(SpriteInstance* aSprite)
{
	myRendreManarger->AddExtraSpriteToRender(aSprite);
}

#if USEIMGUI
void CGraphicsEngine::Imgui()
{
	myRendreManarger->Imgui();
}
#endif // !_RETAIL
DirectX11Framework* CGraphicsEngine::GetFrameWork()
{
	return myFrameWork;
}

LightLoader* CGraphicsEngine::GetLightLoader()
{
	return myLightLoader;
}

SpriteRenderer* CGraphicsEngine::GetSpriteRenderer()
{
	return myRendreManarger->GetSpriteRenderer();
}

RenderManager* CGraphicsEngine::GetRendreManarger()
{
	return myRendreManarger;
}

void CGraphicsEngine::SubscribeToMessages()
{
	myRendreManarger->SubscribeToMessages();
	myWindowHandler->SubscribeToMessages();
}

void CGraphicsEngine::UnsubscribeToMessages()
{
	myRendreManarger->UnsubscribeToMessages();
	myWindowHandler->UnSubscrideToMessages();
}

ModelLoader& CGraphicsEngine::GetModelLoader()
{
	return myModelLoader;
}

SpriteFactory& CGraphicsEngine::GetSpriteFactory()
{
	return mySpriteFactory;
}

WindowHandler* CGraphicsEngine::GetWindowHandler()
{
	return myWindowHandler;
}
