#include "pch.h"
#include "pch.h"

#include "GraphicEngine.h"
#include "Scene.h"
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
	myRendreManarger = new RenderManager();
	myLightLoader = new LightLoader();
}

CGraphicsEngine::~CGraphicsEngine()
{
	myRendreManarger->Release();

	delete myFrameWork;
	myFrameWork = nullptr;
	SAFE_DELETE(myRendreManarger);
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
	myRendreManarger->BeginFrame(aClearColor);
}

void CGraphicsEngine::RenderFrame()
{
	myRendreManarger->Render();
}

void CGraphicsEngine::RenderText(const std::vector<TextInstance*>& aTextList)
{
	myRendreManarger->RenderText(aTextList);
}

void CGraphicsEngine::EndFrame()
{
	myRendreManarger->EndFrame();
	myFrameWork->EndFrame();
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

bool CGraphicsEngine::InitInternal()
{
	if (!myFrameWork->Init())
	{
		SYSCRASH("Could not init graphics framework :c");
		return false;
	}

	AssetManager::GetInstance().Init(myFrameWork->GetDevice(), "../assets", "../baked");
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
	if (!myRendreManarger->Init(myFrameWork))
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
	return myRendreManarger->GetSpriteRenderer();
}

RenderManager* CGraphicsEngine::GetRendreManarger()
{
	return myRendreManarger;
}

SpriteFactory& CGraphicsEngine::GetSpriteFactory()
{
	return mySpriteFactory;
}
