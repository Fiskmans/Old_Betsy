#include "engine/graphics/RenderManager.h"
#include "engine/graphics/TextureFactory.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/GraphicsFramework.h"
#include "engine/graphics/RenderScene.h"
#include "engine/graphics/ShaderMappings.h"

#include "engine/utilities/Stopwatch.h"

#include "engine/GameEngine.h"

#include "imgui/imgui.h"

#include "tools/Logger.h"

#include "tools/ImGuiHelpers.h"
#include "tools/Functors.h"


namespace engine::graphics
{
	RenderManager::TextureMapping::TextureMapping(const AssetHandle<TextureAsset>& aHandle, size_t aIndex)
		: myResource(aHandle.Access().myTexture)
		, mySlot(aIndex)
	{
	}

	RenderManager::RenderManager()
	{
	}

	bool RenderManager::Init()
	{
		if (!myRenderStateManager.Init())
			return false;
		
		if (!myFullscreenRenderer.Init())
			return false;

		if (!CreateTextures(WindowManager::GetInstance().GetSize()))
		{
			return false;
		}

		myResolutionChangedEventHandle = WindowManager::GetInstance().ResolutionChanged.Register(
			[](tools::V2ui aResolution)
			{
				GetInstance().Resize(aResolution);
			});

		return true;
	}

	void RenderManager::Render()
	{
		tools::V4f transparent = tools::V4f(0.f, 0.f, 0.f, 0.f);

		myTextures[static_cast<int>(Channel::BackBuffer)].ClearTexture(transparent);
		myTextures[static_cast<int>(Channel::IntermediateTexture)].ClearTexture();

		RenderScene& scene = GameEngine::GetInstance().GetMainScene();
		Camera* camera = scene.GetMainCamera();

		if (!camera)
		{
			EXECUTE_ONCE({ LOG_ERROR("Main scene has no camera"); });
			return;
		}

		RenderCamera(*camera);

		tools::ImguiHelperGlobals::ResetCounter();
	}

	void RenderManager::RenderCamera(Camera& aCamera)
	{
		myDepthTexture.ClearDepth();

		myTextures[static_cast<int>(Channel::IntermediateTexture)].ClearTexture();

		myTextures[static_cast<int>(Channel::IntermediateTexture)].SetAsResourceOnSlot(0);
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

	void RenderManager::Imgui()
	{

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

	bool RenderManager::SetShaderResource(size_t aSlot, AssetHandle<TextureAsset>& aAsset)
	{
		if (aAsset.IsValid())
		{
			ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
			context->PSSetShaderResources(aSlot, 1, &aAsset.Access().myTexture);
			context->VSSetShaderResources(aSlot, 1, &aAsset.Access().myTexture);
			return true;
		}
		return false;
	}


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


		myGBuffer = TextureFactory::GetInstance().CreateGBuffer(aSize, "Engine GBuffer");
		myDepthTexture = TextureFactory::GetInstance().CreateDepth(aSize, "Engine Depth");

		return true;
	}

	void RenderManager::Resize(const tools::V2ui& aSize)
	{
		GraphicsFramework& framework = GraphicsEngine::GetInstance().GetFrameWork();
		framework.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);

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