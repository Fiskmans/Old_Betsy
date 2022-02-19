#include "pch.h"
#include "pch.h"

#include "GraphicEngine.h"
#include "RenderScene.h"
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

#include "AssetManager.h"

CGraphicsEngine::CGraphicsEngine()
{
	myFrameWork = new DirectX11Framework();
	myLightLoader = new LightLoader();
}

CGraphicsEngine::~CGraphicsEngine()
{

	delete myFrameWork;
	myFrameWork = nullptr;
	SAFE_DELETE(myLightLoader);
}

bool CGraphicsEngine::Init()
{
	if (!WindowHandler::GetInstance().OpenWindow())
	{
		SYSCRASH("Could not init window :c");
		return false;
	}
	return InitInternal();
}

bool CGraphicsEngine::Init(V2ui aWindowSize)
{
	if (!WindowHandler::GetInstance().OpenWindow(aWindowSize))
	{
		SYSCRASH("Could not init window :c");
		return false;
	}
	return InitInternal();
}

void CGraphicsEngine::BeginFrame(float aClearColor[4])
{
	RenderManager::GetInstance().BeginFrame(aClearColor);
}

void CGraphicsEngine::RenderFrame()
{
	RenderManager::GetInstance().Render();
}

void CGraphicsEngine::RenderText(const std::vector<TextInstance*>& aTextList)
{
	RenderManager::GetInstance().RenderText(aTextList);
}

void CGraphicsEngine::EndFrame()
{
	RenderManager::GetInstance().EndFrame();
	myFrameWork->EndFrame();
}

void CGraphicsEngine::AddExtraSpriteToRender(SpriteInstance* aSprite)
{
	RenderManager::GetInstance().AddExtraSpriteToRender(aSprite);
}

#if USEIMGUI
void CGraphicsEngine::Imgui()
{
	RenderManager::GetInstance().Imgui();
}

bool CGraphicsEngine::InitInternal()
{
	if (!myFrameWork->Init())
	{
		SYSCRASH("Could not init graphics framework :c");
		return false;
	}

	AssetManager::GetInstance().Init(myFrameWork->GetDevice(), "../assets", "../baked");
#ifdef _DEBUG
	AssetManager::GetInstance().Preload();
#endif // _DEBUG


	CCameraFactory::Init(NEARPLANE, FARPLANE);

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
	if (!RenderManager::GetInstance().Init(myFrameWork))
	{
		SYSCRASH("Could not init rendremanargre :c");
		return false;
	}

	return true;
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
	return RenderManager::GetInstance().GetSpriteRenderer();
}

SpriteFactory& CGraphicsEngine::GetSpriteFactory()
{
	return mySpriteFactory;
}
