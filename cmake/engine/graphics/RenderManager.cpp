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


namespace engine::graphics
{
	RenderManager::TextureMapping::TextureMapping(const AssetHandle<TextureAsset>& aHandle, size_t aIndex)
		: myResource(aHandle.Access().myTexture)
		, mySlot(aIndex)
	{
	}

	RenderManager::RenderManager()
		//: myStateManager(RenderStateManager())
	{
	}

	bool RenderManager::Init()
	{
		if (!myRenderStateManager.Init())
			return false;
		
		if (!myFullscreenRenderer.Init())
			return false;

		if (!myDeferredRenderer.Init())
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

		myTextures[static_cast<int>(Channel::BackBuffer)].ClearTexture(transparent);
		myTextures[static_cast<int>(Channel::IntermediateTexture)].ClearTexture(aClearColor);
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
			FullscreenPass({ Channel::IntermediateTexture }, Channel::BackBuffer, FullscreenRenderer::Shader::COPY);
			return;
		}

		UnbindTargets();
		UnbindResources();
		
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->PSSetShaderResources(0, 1, &textureHandle.Access().myTexture);

		myTextures[static_cast<int>(Channel::BackBuffer)].SetAsActiveTarget();
		myFullscreenRenderer.Render(FullscreenRenderer::Shader::COPY);
	}

	void RenderManager::MapTextures(AssetHandle<DrawableTextureAsset>& aTarget, const std::vector<TextureMapping>& aTextures, AssetHandle<DepthTextureAsset> aDepth)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();

		UnbindTargets();
		UnbindResources();

		aTarget.Access().myDrawableTexture.SetAsActiveTarget(&aDepth.Access().myTexture);
		for (const TextureMapping& tex : aTextures)
			context->PSSetShaderResources(tex.mySlot, 1, &tex.myResource);
	}

	void RenderManager::MapTextures(AssetHandle<GBufferAsset>& aTarget, const std::vector<TextureMapping>& aTextures, AssetHandle<DepthTextureAsset> aDepth)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();

		UnbindTargets();
		UnbindResources();

		aTarget.Access().myGBuffer.SetAsActiveTarget(&aDepth.Access().myTexture);
		for (const TextureMapping& tex : aTextures)
			context->PSSetShaderResources(tex.mySlot, 1, &tex.myResource);
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

	bool RenderManager::CreateGenericShaderBuffer(ID3D11Buffer*& aBuffer, size_t aSize)
	{
		D3D11_BUFFER_DESC bufferDescription;
		ZeroMemory(&bufferDescription, sizeof(bufferDescription));
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = aSize;

		return SUCCEEDED(GraphicsEngine::GetInstance().GetFrameWork().GetDevice()->CreateBuffer(&bufferDescription, nullptr, &aBuffer));
	}

	bool RenderManager::OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();

		D3D11_MAPPED_SUBRESOURCE bufferData;
		WIPE(bufferData);

		HRESULT result = context->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

		if (FAILED(result))
		{
			LOG_ERROR("Failed to map buffer");
			return false;
		}

		memcpy(bufferData.pData, aData, aSize);
		context->Unmap(aBuffer, 0);

		return true;
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
			LOG_SYS_CRASH("Failed to get get back buffer texture");
			return false;
		}

		TextureFactory::GetInstance().CreateTexture(backBufferTexture, myTextures[static_cast<int>(Channel::BackBuffer)]);

		myTextures[static_cast<int>(Channel::IntermediateTexture)] = TextureFactory::GetInstance().CreateTexture(aSize, DXGI_FORMAT_R8G8B8A8_UNORM, "Intermediate texture");

		for (Texture& tex : myTextures)
		{
			if (!tex.IsValid())
			{
				LOG_SYS_CRASH("Failed to initialize all fullscreen textures");
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
		myTextures[static_cast<int>(Channel::BackBuffer)].SetAsActiveTarget();
	}
		
	void RenderManager::FullscreenPass(std::vector<Channel> aSources, Channel aTarget, FullscreenRenderer::Shader aShader)
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