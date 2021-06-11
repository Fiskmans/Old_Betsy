#include "pch.h"
#include "RenderManager.h"
#include "PostMaster.hpp"
#include "Camera.h"

#include "ParticleFactory.h"
#include "ShaderFlags.h"
#include "ModelInstance.h"
#include "Model.h"

#include "AssetManager.h"

float RenderManager::ourTotalTime = 0.f;

ID3D11DeviceContext* GetAndOrSetContext(ID3D11DeviceContext* aContext = nullptr, bool aReallyShouldSet = false)
{
	static ID3D11DeviceContext* context = nullptr;
	if (aContext || aReallyShouldSet)
	{
		context = aContext;
	}
	return context;
}

ID3D11Resource* GetAndOrSetResource(ID3D11Resource* aResource = nullptr, bool aReallyShouldSet = false)
{
	static ID3D11Resource* resource = nullptr;
	if (aResource || aReallyShouldSet)
	{
		resource = aResource;
	}
	return resource;
}

void SetScreendumpTexture(ID3D11DeviceContext* aContext, ID3D11Resource* aResource)
{
	GetAndOrSetContext(aContext);
	GetAndOrSetResource(aResource);
}

void ClearScreendumpTexture()
{
	if (GetAndOrSetResource())
	{
		GetAndOrSetResource()->Release();
	}

	GetAndOrSetContext(nullptr, true);
	GetAndOrSetResource(nullptr, true);
}

RenderManager::RenderManager() :
	myFrameworkPtr(nullptr),
#if USEIMGUI
	myShouldRenderWireFrame(false),
#endif // USEIMGUI
	myStateManerger(RenderStateManager()),
	myStartedAt(0),
	Observer({
			MessageType::WindowResize
		})
{
}

bool RenderManager::Init(DirectX11Framework* aFramework)
{
	if (!myStateManerger.Init(aFramework))
	{
		return false;
	}
	myPerlinView = AssetManager::GetInstance().GetPerlinTexture({ 1024,1024 }, { 2,2 }, 2);

	if (!myForwardRenderer.Init(aFramework, myPerlinView, &myShadowRenderer))
	{
		return false;
	}
	if (!myFullscreenRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myFullscreenFactory.Init(aFramework))
	{
		return false;
	}
	if (!mySpriteRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myParticleRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myDeferredRenderer.Init(aFramework, myPerlinView, &myShadowRenderer))
	{
		return false;
	}
	if (!myTextRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myHighlightRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myShadowRenderer.Init(aFramework))
	{
		return false;
	}

	DebugDrawer::GetInstance().Init(aFramework);
	DebugDrawer::GetInstance().SetColor(V4F(0.8f, 0.2f, 0.2f, 1.f));
	myFrameworkPtr = aFramework;
	if (!CreateTextures(WindowHandler::GetInstance().GetSize()))
	{
		return false;
	}

	myStartedAt = Tools::GetTotalTime();
#if USEIMGUI
	Tools::ImguiHelperGlobals::Setup(aFramework->GetDevice(), aFramework->GetContext(), &myFullscreenRenderer, &myFullscreenFactory);
#endif // USEIMGUI

	return true;
}

bool RenderManager::Release()
{
	myGBuffer.Release();
	myBufferGBuffer.Release();
	for (auto& i : myTextures)
	{
		i.Release();
	}
	return true;
}

void RenderManager::BeginFrame(float aClearColor[4])
{
	myTextures[static_cast<int>(Textures::BackBuffer)].ClearTexture({ 0 });
	myTextures[static_cast<int>(Textures::IntermediateTexture)].ClearTexture({ aClearColor[0],aClearColor[1],aClearColor[2],aClearColor[3] });
	myTextures[static_cast<int>(Textures::IntermediateDepth)].ClearDepth();
	myTextures[static_cast<int>(Textures::Selection)].ClearTexture({ 0 });
	myTextures[static_cast<int>(Textures::Selection2)].ClearTexture({ 0 });
	myTextures[static_cast<int>(Textures::SelEdges)].ClearTexture({ 0 });
	myTextures[static_cast<int>(Textures::SelEdgesHalf)].ClearTexture({ 0 });
	myTextures[static_cast<int>(Textures::BackFaceBuffer)].ClearTexture({ 0 });
	myGBuffer.ClearTextures();
	myBufferGBuffer.ClearTextures();

	myExtraSpritesToRenderThisFrame.clear();
}

void RenderManager::EndFrame()
{
#if USEIMGUI
	Tools::ImguiHelperGlobals::ResetCounter();
#endif // USEIMGUI
}

void RenderManager::Render()
{
	ourTotalTime = Tools::GetTotalTime() - myStartedAt;

	static std::vector<ModelInstance*> modelsLeftToRender;
	static std::vector<SpriteInstance*> sprites;
	static std::vector<std::array<PointLight*, NUMBEROFPOINTLIGHTS>> affectingLights;
	{
		PERFORMANCETAG("Culling");
		modelsLeftToRender = Scene::GetInstance().Cull(Scene::GetInstance().GetMainCamera());
	}
	std::vector<ModelInstance*> modelsToHighlight;
	std::vector<ModelInstance*> shadowCasters = modelsLeftToRender;
	for (auto& mod : modelsLeftToRender)
	{
		if (mod->GetIsHighlighted())
		{
			modelsToHighlight.push_back(mod);
		}
	}
	sprites = Scene::GetInstance().GetSprites();
	for (auto& sprite : myExtraSpritesToRenderThisFrame)
	{
		sprites.push_back(sprite);
	}

	affectingLights.clear();
	{
		PERFORMANCETAG("BoneTexture Generation");
		static std::vector<ModelInstance*> buffer;
		buffer.clear();
		std::copy_if(Scene::GetInstance().begin(), Scene::GetInstance().end(), std::back_inserter(buffer), [](ModelInstance* aInstance) { return !!(aInstance->GetModelAsset().GetAsModel()->myAnimations.IsValid()); });
		SetupBoneTexture(buffer);
	}



	myStateManerger.SetAllStates();
#if USEIMGUI
	if (myShouldRenderWireFrame)
	{
		myStateManerger.SetRasterizerState(RenderStateManager::RasterizerState::Wireframe);;
	}
#endif // !_RETAIL
	{
		PERFORMANCETAG("Deferred Gbuffer");
		UnbindTargets();
		UnbindResources();
		myStateManerger.SetDepthStencilState(RenderStateManager::DepthStencilState::Default);
		myFrameworkPtr->GetContext()->VSSetShaderResources(5, 1, &myBoneTextureView);
		modelsLeftToRender = myDeferredRenderer.GenerateGBuffer(Scene::GetInstance().GetMainCamera(), modelsLeftToRender, myBoneBuffer, myBoneOffsetMap, &myTextures[static_cast<int>(Textures::BackFaceBuffer)], &myStateManerger, Scene::GetInstance().GetDecals(), &myGBuffer, &myBufferGBuffer, myFullscreenRenderer, &myTextures[ENUM_CAST(Textures::IntermediateDepth)], myBoneBuffer);
		myStateManerger.SetAllStates();
	}

#if ENABLESSAO
	if (myDoSSAO)
	{
		PERFORMANCETAG("SSAO");
		UnbindTargets();
		UnbindResources();
		myGBuffer.SetAllAsResources();

		ID3D11ShaderResourceView* texture[1] =
		{
			myRandomNormal.GetAsTexture()
		};

		myFrameworkPtr->GetContext()->PSSetShaderResources(9, 1, texture);
		myDeferredRenderer.MapEnvLightBuffer();
		myTextures[static_cast<int>(Textures::BackFaceBuffer)].SetAsResourceOnSlot(10);
		myTextures[ENUM_CAST(Textures::IntermediateDepth)].SetAsResourceOnSlot(8);
		myTextures[static_cast<int>(Textures::SSAOBuffer)].SetAsActiveTarget();

		myFullscreenRenderer.Render(FullscreenRenderer::Shader::SSAO);

		UnbindTargets();
		UnbindResources();
		myTextures[static_cast<int>(Textures::SSAOBuffer)].SetAsResourceOnSlot(0);
		myGBuffer.SetAsActiveTarget(GBuffer::Textures::AmbientOcclusion);

		myFullscreenRenderer.Render(FullscreenRenderer::Shader::COPY);
	}
#endif

	{
		PERFORMANCETAG("Deferred render");
		UnbindTargets();
		myGBuffer.SetAllAsResources();
		myTextures[static_cast<int>(Textures::IntermediateTexture)].SetAsActiveTarget();
		myTextures[ENUM_CAST(Textures::IntermediateDepth)].SetAsResourceOnSlot(15);
		myStateManerger.SetSamplerState(RenderStateManager::SamplerState::Point);
		myStateManerger.SetRasterizerState(RenderStateManager::RasterizerState::Default);
		myDeferredRenderer.Render(myFullscreenRenderer, Scene::GetInstance().GetPointLights(), Scene::GetInstance().GetSpotLights(), &myStateManerger, myBoneBuffer, myBoneOffsetMap);
		myStateManerger.SetSamplerState(RenderStateManager::SamplerState::Trilinear);
	}

	for (auto& model : modelsLeftToRender)
	{
		affectingLights.push_back(Scene::GetInstance().CullPointLights(model));
	}

	ModelInstance* skybox = Scene::GetInstance().GetSkybox();
	if (skybox)
	{
		myForwardRenderer.SetSkybox(skybox);
	}


	myStateManerger.SetAllStates();
#if USEIMGUI
	if (myShouldRenderWireFrame)
	{
		myStateManerger.SetRasterizerState(RenderStateManager::RasterizerState::Wireframe);;
	}
#endif // !_RETAIL
	{
		PERFORMANCETAG("Forward render");
		UnbindResources();
		myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		//myStateManerger.SetDepthStencilState(RenderStateManager::DepthStencilState::ReadOnly);
		myGBuffer.SetAsResourceOnSlot(GBuffer::Textures::Postion, 8);
		myTextures[static_cast<int>(Textures::IntermediateTexture)].SetAsActiveTarget(&myTextures[static_cast<int>(Textures::IntermediateDepth)]);
		myForwardRenderer.Render(modelsLeftToRender, Scene::GetInstance().GetMainCamera(), affectingLights, myBoneOffsetMap, myStateManerger, myBoneBuffer);
		myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
	}

	{
		PERFORMANCETAG("Particles");
		myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		myStateManerger.SetDepthStencilState(RenderStateManager::DepthStencilState::ReadOnly);
		ID3D11ShaderResourceView* arr[16] = { nullptr };
		myFrameworkPtr->GetContext()->PSSetShaderResources(0, 16, arr);
		myTextures[static_cast<int>(Textures::IntermediateTexture)].SetAsActiveTarget(&myTextures[static_cast<int>(Textures::IntermediateDepth)]);
		myParticleRenderer.Render(Scene::GetInstance().GetMainCamera(), Scene::GetInstance().GetParticles());
	}


#if USEDEBUGLINES
	if (myDoDebugLines)
	{
		PERFORMANCETAG("Debuglines");
		myStateManerger.SetAllStates();
		myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		myTextures[static_cast<int>(Textures::IntermediateTexture)].SetAsActiveTarget(&myTextures[static_cast<int>(Textures::IntermediateDepth)]);
		DebugDrawer::GetInstance().Render(Scene::GetInstance().GetMainCamera());
		myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
	}
#endif


#if ENABLEBLOOM
	{
		PERFORMANCETAG("Bloom");
		FullscreenPass({ Textures::IntermediateTexture }, Textures::Luminance, FullscreenRenderer::Shader::LUMINANCE);
		FullscreenPass({ Textures::Luminance }, Textures::HalfSize, FullscreenRenderer::Shader::COPY);
		FullscreenPass({ Textures::HalfSize }, Textures::QuaterSize, FullscreenRenderer::Shader::COPY);
		FullscreenPass({ Textures::QuaterSize }, Textures::HalfQuaterSize, FullscreenRenderer::Shader::COPY);
		FullscreenPass({ Textures::HalfQuaterSize }, Textures::Guassian1, FullscreenRenderer::Shader::GAUSSIANHORIZONTAL);
		FullscreenPass({ Textures::Guassian1 }, Textures::Guassian2, FullscreenRenderer::Shader::GAUSSIANVERTICAL);
		FullscreenPass({ Textures::Guassian2 }, Textures::HalfQuaterSize, FullscreenRenderer::Shader::COPY);
		FullscreenPass({ Textures::HalfQuaterSize }, Textures::QuaterSize, FullscreenRenderer::Shader::COPY);
		FullscreenPass({ Textures::QuaterSize }, Textures::HalfSize, FullscreenRenderer::Shader::MERGE);
		FullscreenPass({ Textures::IntermediateTexture ,Textures::HalfSize }, Textures::BackBuffer, FullscreenRenderer::Shader::MERGE);
	}
#endif

	if (myDoAA)
	{
		myStateManerger.SetAllStates();
		myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
		PERFORMANCETAG("AA");
		FullscreenPass({ Textures::IntermediateTexture }, Textures::Edges, FullscreenRenderer::Shader::EdgeDetection);
		FullscreenPass({ Textures::IntermediateTexture ,Textures::Edges }, Textures::AAHorizontal, FullscreenRenderer::Shader::ConditionalGAUSSIANHORIZONTAL);
		FullscreenPass({ Textures::AAHorizontal ,Textures::Edges }, Textures::BackBuffer, FullscreenRenderer::Shader::ConditionalGAUSSIANVERTICAL);
	}
	else
	{
		FullscreenPass({ Textures::IntermediateTexture }, Textures::BackBuffer, FullscreenRenderer::Shader::COPY);
	}

	RenderSelection(modelsToHighlight, Scene::GetInstance().GetMainCamera());

	RenderSprites(sprites);
	RenderText(Scene::GetInstance().GetText());
}

void RenderManager::RenderSprites(const std::vector<SpriteInstance*>& aSpriteList)
{
	if (aSpriteList.size() == 0)
	{
		return;
	}
	myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();
	myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
	myStateManerger.SetRasterizerState(RenderStateManager::RasterizerState::NoBackfaceCulling);
	myStateManerger.SetDepthStencilState(RenderStateManager::DepthStencilState::Default);

	mySpriteRenderer.Render(aSpriteList);

	myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
	myStateManerger.SetRasterizerState(RenderStateManager::RasterizerState::Default);
}

void RenderManager::RenderText(const std::vector<TextInstance*>& aTextList)
{
	myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();
	myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
	myStateManerger.SetRasterizerState(RenderStateManager::RasterizerState::NoBackfaceCulling); //Not sure if needed :o

	myTextRenderer.Render(aTextList);

	myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
	myStateManerger.SetRasterizerState(RenderStateManager::RasterizerState::Default);

}

void RenderManager::AddExtraSpriteToRender(SpriteInstance* aSprite)
{
	if (aSprite)
	{
		myExtraSpritesToRenderThisFrame.push_back(aSprite);
		return;
	}

	SYSWARNING("Tried to add null sprite to render list");
}

void RenderManager::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::WindowResize:
		Resize(*reinterpret_cast<const V2ui*>(aMessage.myData));
		break;
	default:
		break;
	}
}

void RenderManager::SetupBoneTexture(const std::vector<ModelInstance*>& aModelList)
{
	static_assert(sizeof(char) == 1 && CHAR_BIT == 8, "Look over this code, make sure it works with this newfangled bytesize of yours... crazy why are you still looking at this code btw it's been decenia.. move on");

	myBoneOffsetMap.clear();


	std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES> matrixes;
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

SpriteRenderer* RenderManager::GetSpriteRenderer()
{
	return &mySpriteRenderer;
}

float RenderManager::GetTotalTime()
{
	return ourTotalTime;
}

#if USEIMGUI
void RenderManager::Imgui()
{
	ImGui::Checkbox("Wireframe", &myShouldRenderWireFrame);
	ImGui::BeginGroup();
#if ENABLESSAO
	ImGui::Checkbox("SSAO", &myDoSSAO);
#endif
	ImGui::Checkbox("Anti aliasing", &myDoAA);
	ImGui::Separator();
	Tools::ZoomableImGuiSnapshot(myTextures[static_cast<int>(Textures::BackBuffer)].GetResourceView(),ImVec2(192*2,108*2));
	ImGui::Separator();

	ImGui::EndGroup();

}
#endif // !_RETAIL


void RenderManager::RenderSelection(const std::vector<ModelInstance*>& aModelsToHighlight, Camera* aCamera)
{
	myStateManerger.SetAllStates();
	myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
	//return;
	myTextures[static_cast<int>(Textures::Selection)].SetAsActiveTarget();
	myHighlightRenderer.Render(aModelsToHighlight, aCamera, myBoneBuffer, myBoneOffsetMap);

	FullscreenPass({ Textures::Selection }, Textures::SelectionScaleDown1, FullscreenRenderer::Shader::COPY);
	FullscreenPass({ Textures::SelectionScaleDown1 }, Textures::SelEdgesHalf, FullscreenRenderer::Shader::GAUSSIANVERTICAL);
	FullscreenPass({ Textures::SelEdgesHalf }, Textures::SelEdges, FullscreenRenderer::Shader::GAUSSIANHORIZONTAL);
	FullscreenPass({ Textures::SelEdges }, Textures::SelectionScaleDown1, FullscreenRenderer::Shader::COPY);
	FullscreenPass({ Textures::Selection, Textures::SelectionScaleDown1 }, Textures::Selection2, FullscreenRenderer::Shader::MERGE);

	myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
	FullscreenPass({ Textures::Selection2 }, Textures::BackBuffer, FullscreenRenderer::Shader::DiscardFull);
	myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
}

bool RenderManager::CreateTextures(const V2ui& aSize)
{
	ID3D11Texture2D* backBufferTexture = myFrameworkPtr->GetBackbufferTexture();
	if (!backBufferTexture)
	{
		SYSERROR("Could not get back buffer texture");
		return false;
	}

	myTextures[static_cast<int>(Textures::BackBuffer)] = myFullscreenFactory.CreateTexture(backBufferTexture);
	myTextures[static_cast<int>(Textures::IntermediateDepth)] = myFullscreenFactory.CreateDepth({ aSize.x, aSize.y }, "Main Depth");

	myTextures[static_cast<int>(Textures::IntermediateTexture)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8G8B8A8_UNORM, "Intermediate texture");
	myTextures[static_cast<int>(Textures::HalfSize)] = myFullscreenFactory.CreateTexture({ aSize.x / 2U, aSize.y / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/2 size");
	myTextures[static_cast<int>(Textures::QuaterSize)] = myFullscreenFactory.CreateTexture({ aSize.x / 4U, aSize.y / 4U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/4 size");
	myTextures[static_cast<int>(Textures::HalfQuaterSize)] = myFullscreenFactory.CreateTexture({ aSize.x / 8U, aSize.y / 8U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/8 size");

	myTextures[static_cast<int>(Textures::SSAOBuffer)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8_UNORM, "SSAO buffer");
	myTextures[static_cast<int>(Textures::BackFaceBuffer)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8G8B8A8_UNORM, "Backface buffer");

#if ENABLEBLOOM
	myTextures[static_cast<int>(Textures::Guassian1)] = myFullscreenFactory.CreateTexture({ aSize.x / 8U, aSize.y / 8U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom guassian 1");
	myTextures[static_cast<int>(Textures::Guassian2)] = myFullscreenFactory.CreateTexture({ aSize.x / 8U, aSize.y / 8U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom guassian 2");

	myTextures[static_cast<int>(Textures::Luminance)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8G8B8A8_UNORM, "luminance");
#endif

	myTextures[static_cast<int>(Textures::SelectionScaleDown1)] = myFullscreenFactory.CreateTexture({ aSize.x / 2U, aSize.y / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 1/2");
	myTextures[static_cast<int>(Textures::SelectionScaleDown2)] = myFullscreenFactory.CreateTexture({ aSize.x / 4U, aSize.y / 4U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 1/4");

	myTextures[static_cast<int>(Textures::Selection)] = myFullscreenFactory.CreateTexture({ aSize.x / 2U, aSize.y / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection");
	myTextures[static_cast<int>(Textures::SelEdgesHalf)] = myFullscreenFactory.CreateTexture({ aSize.x / 2U, aSize.y / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection edges half");
	myTextures[static_cast<int>(Textures::SelEdges)] = myFullscreenFactory.CreateTexture({ aSize.x / 2U, aSize.y / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection edges all");
	myTextures[static_cast<int>(Textures::Selection2)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 2");

	myTextures[static_cast<int>(Textures::Edges)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8_UNORM, "Edges");
	myTextures[static_cast<int>(Textures::AAHorizontal)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8G8B8A8_UNORM, "AA horisontal");

	myTextures[static_cast<int>(Textures::LUT)] = myFullscreenFactory.CreateTexture({ aSize.x, aSize.y }, DXGI_FORMAT_R8G8B8A8_UNORM, "LUT buffer");

	myRandomNormal = AssetManager::GetInstance().GetTexture("engine/SSAONormal.dds");
	myGBuffer = myFullscreenFactory.CreateGBuffer({ aSize.x,aSize.y }, "main gBuffer");
	myBufferGBuffer = myFullscreenFactory.CreateGBuffer({ aSize.x,aSize.y }, "secondary GBuffer");

	for (auto& i : myTextures)
	{
		if (!i)
		{
			SYSERROR("Could not initialize all fullscreen textures");
			return false;
		}
	}


	ID3D11Device* device = myFrameworkPtr->GetDevice();



	const size_t bytesPerPixel = sizeof(V4F);
	const size_t width = sizeof(CommonUtilities::Matrix4x4<float>) * NUMBEROFANIMATIONBONES / bytesPerPixel;
	const size_t height = MAXNUMBEROFANIMATIONSONSCREEN;
	CD3D11_TEXTURE2D_DESC desc;
	WIPE(desc);
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MiscFlags = 0;

	static_assert(sizeof(BoneTextureCPUBuffer) == (width * height * bytesPerPixel) && "Something got missmatched");

	DirectX11Framework::AddGraphicsMemoryUsage(static_cast<size_t>(sizeof(BoneTextureCPUBuffer) * DirectX11Framework::FormatToSizeLookup[desc.Format]), "Bone Texture", "Engine Texture");


	HRESULT result = device->CreateTexture2D(&desc, nullptr, &myBoneBufferTexture);
	if (FAILED(result))
	{
		SYSERROR("Could not create bone buffer texture");
		return false;
	}


	result = device->CreateShaderResourceView(myBoneBufferTexture, nullptr, &myBoneTextureView);
	if (FAILED(result))
	{
		SYSERROR("Could not create bone shader resource view");
		return false;
	}

	return true;
}

void RenderManager::Resize(const V2ui& aSize)
{
	myFrameworkPtr->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);

	for (auto& tex : myTextures)
	{
		tex.Release();
	}
	myGBuffer.Release();
	myBufferGBuffer.Release();
	ClearScreendumpTexture();


	HRESULT result;
	IDXGISwapChain* chain = myFrameworkPtr->GetSwapChain();
	if (chain)
	{
		result = chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(result))
		{
			SYSERROR("Could not resize swap chain buffer.");
		}
	}

	CreateTextures(aSize);
	myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();

	SetScreendumpTexture(myFrameworkPtr->GetContext(), myFrameworkPtr->GetBackbufferTexture());
}

void RenderManager::FullscreenPass(std::vector<Textures> aSources, Textures aTarget, FullscreenRenderer::Shader aShader)
{
	UnbindTargets();
	UnbindResources();
	for (size_t i = 0; i < aSources.size(); i++)
	{
		myTextures[static_cast<int>(aSources[i])].SetAsResourceOnSlot(i);
	}
	myTextures[static_cast<int>(aTarget)].SetAsActiveTarget();
	myFullscreenRenderer.Render(aShader);
}

void RenderManager::UnbindResources()
{
	ID3D11ShaderResourceView* views[16] = { nullptr };
	myFrameworkPtr->GetContext()->PSSetShaderResources(0, 16, views);
}

void RenderManager::UnbindTargets()
{
	myFrameworkPtr->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
}

