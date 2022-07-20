#include "engine/graphics/RenderManager.h"
#include "engine/graphics/TextureFactory.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/GraphicsFramework.h"
#include "engine/graphics/RenderScene.h"

#include "engine/assets/Model.h"
#include "engine/assets/PointLight.h"

#include "engine/GameEngine.h"

#include "imgui/imgui.h"

#include "logger/Logger.h"

#include "tools/ImGuiHelpers.h"
#include "tools/Functors.h"
#include "tools/TimeHelper.h"


namespace engine
{
	RenderManager::RenderManager()
		//: myStateManager(RenderStateManager())
	{
	}

	bool RenderManager::Init()
	{
		if (!myStateManager.Init())
			return false;
		
		if (!myFullscreenRenderer.Init())
			return false;
		
		//myPerlinView = AssetManager::GetInstance().GetPerlinTexture({ 1024,1024 }, { 2,2 }, 2);
		//
		//if (!myForwardRenderer.Init(aFramework, myPerlinView, &myShadowRenderer))
		//{
		//	return false;
		//}
		//if (!TextureFactory::GetInstance().Init(aFramework))
		//{
		//	return false;
		//}
		//if (!mySpriteRenderer.Init(aFramework))
		//{
		//	return false;
		//}
		//if (!myParticleRenderer.Init(aFramework))
		//{
		//	return false;
		//}
		//if (!myDeferredRenderer.Init(aFramework, myPerlinView, &myShadowRenderer))
		//{
		//	return false;
		//}
		//if (!myTextRenderer.Init(aFramework))
		//{
		//	return false;
		//}
		//if (!myHighlightRenderer.Init(aFramework))
		//{
		//	return false;
		//}
		//if (!myShadowRenderer.Init(aFramework))
		//{
		//	return false;
		//}
		//
		//DebugDrawer::GetInstance().Init(aFramework);
		//DebugDrawer::GetInstance().SetColor(V4F(0.8f, 0.2f, 0.2f, 1.f));
		if (!CreateTextures(WindowManager::GetInstance().GetSize()))
		{
			return false;
		}

		myStartedAt = tools::GetTotalTime();
		myIsReady = true;
		return true;
	}

	void RenderManager::BeginFrame(tools::V4f aClearColor)
	{
		tools::V4f transparent = tools::V4f(0.f, 0.f, 0.f, 0.f);

		myTextures[static_cast<int>(Textures::BackBuffer)].ClearTexture(transparent);
		myTextures[static_cast<int>(Textures::IntermediateTexture)].ClearTexture(aClearColor);
		myTextures[static_cast<int>(Textures::IntermediateDepth)].ClearDepth();
		myTextures[static_cast<int>(Textures::Selection)].ClearTexture(transparent);
		myTextures[static_cast<int>(Textures::Selection2)].ClearTexture(transparent);
		myTextures[static_cast<int>(Textures::SelEdges)].ClearTexture(transparent);
		myTextures[static_cast<int>(Textures::SelEdgesHalf)].ClearTexture(transparent);
		myTextures[static_cast<int>(Textures::BackFaceBuffer)].ClearTexture(transparent);
		myGBuffer.ClearTextures();
		myBufferGBuffer.ClearTextures();
	}

	void RenderManager::EndFrame()
	{
		tools::ImguiHelperGlobals::ResetCounter();
	}

	void RenderManager::Render()
	{
		tools::GetTotalTime();

		RenderScene& scene = GameEngine::GetInstance().GetMainScene();
		Camera* camera = scene.GetMainCamera();

		if (!camera)
		{
			EXECUTE_ONCE({ LOG_ERROR("Main scene has no camera"); });
			return;
		}

		AssetHandle textureHandle = camera->GetTexture();
		if (!textureHandle.IsValid())
		{
			FullscreenPass({ Textures::IntermediateTexture }, Textures::BackBuffer, FullscreenRenderer::Shader::COPY);
			return;
		}

		UnbindTargets();
		UnbindResources();
		
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->PSSetShaderResources(0, 1, &textureHandle.Get<TextureAsset>().myTexture);

		myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();
		myFullscreenRenderer.Render(FullscreenRenderer::Shader::COPY);
	}


	//sprites
		//if (aSpriteList.size() == 0)
		//{
		//	return;
		//}
		//myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();
		//myStateManager.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		//myStateManager.SetRasterizerState(RenderStateManager::RasterizerState::NoBackfaceCulling);
		//myStateManager.SetDepthStencilState(RenderStateManager::DepthStencilState::Default);
		//
		//mySpriteRenderer.Render(aSpriteList);
		//
		//myStateManager.SetBlendState(RenderStateManager::BlendState::Disable);
		//myStateManager.SetRasterizerState(RenderStateManager::RasterizerState::Default);

	//text
		//myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();
		//myStateManager.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		//myStateManager.SetRasterizerState(RenderStateManager::RasterizerState::NoBackfaceCulling); //Not sure if needed :o
		//
		//myTextRenderer.Render(aTextList);
		//
		//myStateManager.SetBlendState(RenderStateManager::BlendState::Disable);
		//myStateManager.SetRasterizerState(RenderStateManager::RasterizerState::Default);

	/*
	void RenderManager::SetupBoneTexture(const std::vector<ModelInstance*>& aModelList)
	{
		static_assert(sizeof(char) == 1 && CHAR_BIT == 8, "Look over this code, make sure it works with this newfangled bytesize of yours... crazy why are you still looking at this code btw it's been decenia.. move on");

		myBoneOffsetMap.clear();

		unsigned char boneIndexOffset = 0;

		ID3D11DeviceContext* context = myFrameworkPtr->GetContext();
		assert(context && "Hol' up, wtf");
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		for (auto& model : aModelList)
		{
			if (boneIndexOffset >= MAXNUMBEROFANIMATIONSONSCREEN)
			{
				break;
			}
			if (model->GetModelAsset().GetAsModel()->ShouldRender() && model->GetModelAsset().GetAsModel()->myAnimations.IsValid())
			{
				std::array<M44f, NUMBEROFANIMATIONBONES>& matrixes = myBoneBuffer[boneIndexOffset];
				for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
				{
					matrixes[i] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				}
				model->SetupanimationMatrixes(matrixes);
				myBoneOffsetMap[model] = boneIndexOffset;
				boneIndexOffset++;
			}
		}
		context->Map(myBoneBufferTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &myBoneBuffer, sizeof(myBoneBuffer));
		context->Unmap(myBoneBufferTexture, 0);
	}
	*/

	void RenderManager::Imgui()
	{
		ImGui::Checkbox("Wire frame", &myShouldRenderWireFrame);
		ImGui::BeginGroup();
		ImGui::Checkbox("Anti aliasing", &myDoAA);
		ImGui::Separator();
		//tools::ZoomableImGuiSnapshot(myTextures[static_cast<int>(Textures::BackBuffer)].GetResourceView(),ImVec2(192*2,108*2));
		ImGui::Separator();

		ImGui::EndGroup();
	}

	/*
	void RenderManager::RenderSelection(const std::vector<ModelInstance*>& aModelsToHighlight, Camera* aCamera)
	{
		myStateManager.SetAllStates();
		myStateManager.SetBlendState(RenderStateManager::BlendState::Disable);
		//return;
		myTextures[static_cast<int>(Textures::Selection)].SetAsActiveTarget();
		myHighlightRenderer.Render(aModelsToHighlight, aCamera, myBoneBuffer, myBoneOffsetMap);

		FullscreenPass({ Textures::Selection }, Textures::SelectionScaleDown1, FullscreenRenderer::Shader::COPY);
		FullscreenPass({ Textures::SelectionScaleDown1 }, Textures::SelEdgesHalf, FullscreenRenderer::Shader::GAUSSIANVERTICAL);
		FullscreenPass({ Textures::SelEdgesHalf }, Textures::SelEdges, FullscreenRenderer::Shader::GAUSSIANHORIZONTAL);
		FullscreenPass({ Textures::SelEdges }, Textures::SelectionScaleDown1, FullscreenRenderer::Shader::COPY);
		FullscreenPass({ Textures::Selection, Textures::SelectionScaleDown1 }, Textures::Selection2, FullscreenRenderer::Shader::MERGE);

		myStateManager.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		FullscreenPass({ Textures::Selection2 }, Textures::BackBuffer, FullscreenRenderer::Shader::DiscardFull);
		myStateManager.SetBlendState(RenderStateManager::BlendState::Disable);
	}
	*/


	bool RenderManager::CreateTextures(const tools::V2ui& aSize)
	{
		ID3D11Texture2D* backBufferTexture = GraphicsEngine::GetInstance().GetFrameWork().GetBackbufferTexture();
		if (!backBufferTexture)
		{
			LOG_SYS_ERROR("Could not get back buffer texture");
			return false;
		}

		TextureFactory::GetInstance().CreateTexture(backBufferTexture, myTextures[static_cast<int>(Textures::BackBuffer)]);
		myTextures[static_cast<int>(Textures::IntermediateDepth)] = TextureFactory::GetInstance().CreateDepth(aSize, "Main Depth");

		myTextures[static_cast<int>(Textures::IntermediateTexture)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8G8B8A8_UNORM, "Intermediate texture");
		myTextures[static_cast<int>(Textures::HalfSize)] = TextureFactory::GetInstance().CreateTexture(aSize / 2, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/2 size");
		myTextures[static_cast<int>(Textures::QuaterSize)] = TextureFactory::GetInstance().CreateTexture(aSize / 4, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/4 size");
		myTextures[static_cast<int>(Textures::HalfQuaterSize)] = TextureFactory::GetInstance().CreateTexture(aSize / 8, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/8 size");

		myTextures[static_cast<int>(Textures::SSAOBuffer)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8_UNORM, "SSAO buffer");
		myTextures[static_cast<int>(Textures::BackFaceBuffer)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8G8B8A8_UNORM, "Backface buffer");

#if ENABLEBLOOM
		myTextures[static_cast<int>(Textures::Guassian1)] = TextureFactory::GetInstance().CreateTexture(aSize / 8, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom Gaussian 1");
		myTextures[static_cast<int>(Textures::Guassian2)] = TextureFactory::GetInstance().CreateTexture(aSize / 8, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom Gaussian 2");

		myTextures[static_cast<int>(Textures::Luminance)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8G8B8A8_UNORM, "luminance");
#endif

		myTextures[static_cast<int>(Textures::SelectionScaleDown1)] = TextureFactory::GetInstance().CreateTexture(aSize / 2, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 1/2");
		myTextures[static_cast<int>(Textures::SelectionScaleDown2)] = TextureFactory::GetInstance().CreateTexture(aSize / 4, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 1/4");

		myTextures[static_cast<int>(Textures::Selection)] = TextureFactory::GetInstance().CreateTexture(aSize / 2, DXGI_FORMAT_R8G8B8A8_UNORM, "selection");
		myTextures[static_cast<int>(Textures::SelEdgesHalf)] = TextureFactory::GetInstance().CreateTexture(aSize / 2, DXGI_FORMAT_R8G8B8A8_UNORM, "selection edges half");
		myTextures[static_cast<int>(Textures::SelEdges)] = TextureFactory::GetInstance().CreateTexture(aSize / 2, DXGI_FORMAT_R8G8B8A8_UNORM, "selection edges all");
		myTextures[static_cast<int>(Textures::Selection2)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 2");

		myTextures[static_cast<int>(Textures::Edges)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8_UNORM, "Edges");
		myTextures[static_cast<int>(Textures::AAHorizontal)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8G8B8A8_UNORM, "AA horizontal");

		myTextures[static_cast<int>(Textures::LUT)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8G8B8A8_UNORM, "LUT buffer");

		//myRandomNormal = AssetManager::GetInstance().GetTexture("engine/SSAONormal.dds");
		myGBuffer = TextureFactory::GetInstance().CreateGBuffer(aSize, "main gBuffer");
		myBufferGBuffer = TextureFactory::GetInstance().CreateGBuffer(aSize, "secondary GBuffer");

		for (Texture& tex : myTextures)
		{
			if (!tex)
			{
				LOG_SYS_CRASH("Could not initialize all fullscreen textures");
				return false;
			}
		}


		//ID3D11Device* device = myFrameworkPtr->GetDevice();

		//const size_t bytesPerPixel = sizeof(tools::V4f);
		//const size_t width = sizeof(CommonUtilities::Matrix4x4<float>) * NUMBEROFANIMATIONBONES / bytesPerPixel;
		//const size_t height = MAXNUMBEROFANIMATIONSONSCREEN;
		//CD3D11_TEXTURE2D_DESC desc;
		//WIPE(desc);
		//desc.Width = width;
		//desc.Height = height;
		//desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//desc.Usage = D3D11_USAGE_DYNAMIC;
		//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//desc.MipLevels = 1;
		//desc.ArraySize = 1;
		//desc.SampleDesc.Count = 1;
		//desc.SampleDesc.Quality = 0;
		//desc.MiscFlags = 0;
		//
		//static_assert(sizeof(BoneTextureCPUBuffer) == (width * height * bytesPerPixel) && "Something got missmatched");
		//
		//GraphicsFramework::AddGraphicsMemoryUsage(static_cast<size_t>(sizeof(BoneTextureCPUBuffer) * GraphicsFramework::FormatToSizeLookup[desc.Format]), "Bone Texture", "Engine Texture");
		//
		//
		//HRESULT result = device->CreateTexture2D(&desc, nullptr, &myBoneBufferTexture);
		//if (FAILED(result))
		//{
		//	SYSERROR("Could not create bone buffer texture");
		//	return false;
		//}
		//
		//
		//result = device->CreateShaderResourceView(myBoneBufferTexture, nullptr, &myBoneTextureView);
		//if (FAILED(result))
		//{
		//	SYSERROR("Could not create bone shader resource view");
		//	return false;
		//}

		return true;
	}

	void RenderManager::Resize(const tools::V2ui& aSize)
	{
		if (!myIsReady)
		{
			return;
		}
		GraphicsFramework& framework = GraphicsEngine::GetInstance().GetFrameWork();
		framework.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);

		for (Texture& tex : myTextures)
		{
			tex.Release();
		}
		myGBuffer.Release();
		myBufferGBuffer.Release();


		HRESULT result;
		IDXGISwapChain* chain = framework.GetSwapChain();
		if (chain)
		{
			result = chain->ResizeBuffers(0, aSize[0], aSize[1], DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Could not resize swap chain buffer.");
				return;
			}
		}

		CreateTextures(aSize);
		myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();
	}
		
	void RenderManager::FullscreenPass(std::vector<Textures> aSources, Textures aTarget, FullscreenRenderer::Shader aShader)
	{
		UnbindTargets();
		UnbindResources();
		for (unsigned int i = 0; i < aSources.size(); i++)
		{
			myTextures[static_cast<int>(aSources[i])].SetAsResourceOnSlot(i);
		}
		myTextures[static_cast<int>(aTarget)].SetAsActiveTarget();
		myFullscreenRenderer.Render(aShader);
	}

	void RenderManager::UnbindResources()
	{
		ID3D11ShaderResourceView* views[16] = { nullptr };
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->PSSetShaderResources(0, 16, views);
	}

	void RenderManager::UnbindTargets()
	{
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}
}