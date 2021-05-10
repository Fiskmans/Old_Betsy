#include "pch.h"
#include "RenderManager.h"
#include "DirectX11Framework.h"
#include "WindowHandler.h"
#include "Scene.h"
#include "Skybox.h"
#include "PostMaster.hpp"
#include <d3d11.h>
#include <imgui.h>
#include "DebugDrawer.h"
#include "Camera.h"

#include "ParticleFactory.h"
#include "ShaderFlags.h"
#include "ModelInstance.h"
#include "Model.h"
#include "TextureLoader.h"

#if USEIMGUI
#include <ImGuiHelpers.h>
#endif

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
	myStartedAt(0)
{
}

bool RenderManager::Init(DirectX11Framework* aFramework, WindowHandler* aWindowHandler)
{
	if (!myStateManerger.Init(aFramework))
	{
		return false;
	}
	myPerlinView = GeneratePerlin(aFramework->GetDevice(), { 1024,1024 }, { 2,2 }, 2);
	myLutTexture1 = LoadTexture(aFramework->GetDevice(),"Data/Textures/SharedTextures/RGBTable16x1_Autumn.dds");
	myLutTexture2 = LoadTexture(aFramework->GetDevice(),"Data/Textures/SharedTextures/RGBTable16x1_Winter.dds");

	if (!myForwardRenderer.Init(aFramework, "Data/Shaders/ThroughWall.hlsl", "Data/Shaders/ThroughWallEnemy.hlsl", &myPerlinView,&myShadowRenderer))
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
	if (!myDeferredRenderer.Init(aFramework, &myPerlinView, &myShadowRenderer))
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
	myScreenSize = { aWindowHandler->GetWidth(), aWindowHandler->GetHeight() };
	if (!CreateTextures(aWindowHandler->GetWidth(), aWindowHandler->GetHeight()))
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

void RenderManager::Render(Scene* aScene)
{
	ourTotalTime = Tools::GetTotalTime() - myStartedAt;

	static std::vector<ModelInstance*> modelsLeftToRender;
	static std::vector<SpriteInstance*> sprites;
	static std::vector<std::array<PointLight*, NUMBEROFPOINTLIGHTS>> affectingLights;
	{
		PERFORMANCETAG("Culling");
		modelsLeftToRender = aScene->Cull(aScene->GetMainCamera());
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
	sprites = aScene->GetSprites();
	for (auto& sprite : myExtraSpritesToRenderThisFrame)
	{
		sprites.push_back(sprite);
	}

	affectingLights.clear();
	{
		PERFORMANCETAG("BoneTexture Generation");
		static std::vector<ModelInstance*> buffer;
		buffer.clear();
		std::copy_if(aScene->begin(), aScene->end(), std::back_inserter(buffer), [](ModelInstance* aInstance) { return !!(aInstance->GetModelAsset().GetAsModel()->GetModelData()->myshaderTypeFlags & ShaderFlags::HasBones); });
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
		modelsLeftToRender = myDeferredRenderer.GenerateGBuffer(aScene->GetMainCamera(), modelsLeftToRender, myBoneOffsetMap, &myTextures[static_cast<int>(Textures::BackFaceBuffer)], &myStateManerger, aScene->GetDecals(), &myGBuffer, &myBufferGBuffer, myFullscreenRenderer, aScene, &myTextures[ENUM_CAST(Textures::IntermediateDepth)], myBoneBuffer);
		myStateManerger.SetAllStates();
	}

#if ENABLESSAO
	if (myDoSSAO)
	{
		PERFORMANCETAG("SSAO");
		myFrameworkPtr->GetContext()->OMSetRenderTargets(8, resetTargets, nullptr);
		myFrameworkPtr->GetContext()->PSSetShaderResources(0, 16, resetViews);
		myGBuffer.SetAllAsResources();
		myFrameworkPtr->GetContext()->PSSetShaderResources(9, 1, *myRandomNormalview);
		myDeferredRenderer.MapEnvLightBuffer(aScene);
		myTextures[static_cast<int>(Textures::BackFaceBuffer)].SetAsResourceOnSlot(10);
		myTextures[ENUM_CAST(Textures::IntermediateDepth)].SetAsResourceOnSlot(8);
		myTextures[static_cast<int>(Textures::SSAOBuffer)].SetAsActiveTarget();

		myFullscreenRenderer.Render(FullscreenRenderer::Shader::SSAO);

		myFrameworkPtr->GetContext()->OMSetRenderTargets(8, resetTargets, nullptr);
		myFrameworkPtr->GetContext()->PSSetShaderResources(0, 16, resetViews);
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
		myDeferredRenderer.Render(myFullscreenRenderer, aScene->GetPointLights(), aScene->GetSpotLights(), aScene, &myStateManerger, myBoneOffsetMap);
		myStateManerger.SetSamplerState(RenderStateManager::SamplerState::Trilinear);
	}

	for (auto& model : modelsLeftToRender)
	{
		affectingLights.push_back(aScene->CullPointLights(model));
	}

	Skybox* skybox = aScene->GetSkybox();
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
		myForwardRenderer.Render(modelsLeftToRender, aScene->GetMainCamera(), aScene, affectingLights, myBoneOffsetMap, myStateManerger, myBoneBuffer);
		myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
	}

	{
		PERFORMANCETAG("LUT");
		myFrameworkPtr->GetContext()->PSSetShaderResources(1, 1, *myLutTexture1);
		myFrameworkPtr->GetContext()->PSSetShaderResources(2, 1, *myLutTexture2);
		myStateManerger.SetSamplerState(RenderStateManager::SamplerState::Point);
		FullscreenPass({ Textures::IntermediateTexture }, Textures::LUT, FullscreenRenderer::Shader::LUT);
		FullscreenPass({ Textures::LUT }, Textures::IntermediateTexture, FullscreenRenderer::Shader::COPY);
	}

	{
		PERFORMANCETAG("Particles");
		myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		myStateManerger.SetDepthStencilState(RenderStateManager::DepthStencilState::ReadOnly);
		ID3D11ShaderResourceView* arr[16] = { nullptr };
		myFrameworkPtr->GetContext()->PSSetShaderResources(0, 16, arr);
		myTextures[static_cast<int>(Textures::IntermediateTexture)].SetAsActiveTarget(&myTextures[static_cast<int>(Textures::IntermediateDepth)]);
		myParticleRenderer.Render(aScene->GetMainCamera(), aScene->GetParticles());
	}


#if USEDEBUGLINES
	if (myDoDebugLines)
	{
		PERFORMANCETAG("Debuglines");
		myStateManerger.SetAllStates();
		myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
		myTextures[static_cast<int>(Textures::IntermediateTexture)].SetAsActiveTarget(&myTextures[static_cast<int>(Textures::IntermediateDepth)]);
		DebugDrawer::GetInstance().Render(aScene->GetMainCamera());
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

	RenderSelection(modelsToHighlight, aScene->GetMainCamera());


	RenderSprites(sprites);
	RenderText(aScene->GetText()); //Todo: probs some culling
}

void RenderManager::RenderMovie(const std::vector<SpriteInstance*>& aSpriteList)
{
	myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();

	myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
	myStateManerger.SetSamplerState(RenderStateManager::SamplerState::Trilinear);
	mySpriteRenderer.Render(aSpriteList);
	myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);

}

void RenderManager::RenderSprites(const std::vector<SpriteInstance*>& aSpriteList, const bool aShouldRenderExtraSprites)
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

	if (aShouldRenderExtraSprites && myExtraSpritesToRenderThisFrame.size() > 0)
	{
		mySpriteRenderer.Render(myExtraSpritesToRenderThisFrame);
	}

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

	SYSWARNING("Tried to add null sprite to render list", "");
}

void RenderManager::SubscribeToMessages()
{
	PostMaster::GetInstance()->Subscribe(MessageType::WindowResize, this);
	PostMaster::GetInstance()->Subscribe(MessageType::NextRenderPass, this);
	myDeferredRenderer.SubscribeToMessages();
	myParticleRenderer.SubscribeToMessages();
	myForwardRenderer.SubscribeToMessages();
}

void RenderManager::UnsubscribeToMessages()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::WindowResize, this);
	PostMaster::GetInstance()->UnSubscribe(MessageType::NextRenderPass, this);
	myDeferredRenderer.UnsubscribeToMessages();
	myParticleRenderer.UnsubscribeToMessages();
	myForwardRenderer.UnsubscribeToMessages();
}

void RenderManager::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::WindowResize:
		Resize(aMessage.myIntValue, aMessage.myIntValue2);
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
		if (model->GetModelAsset().GetAsModel()->ShouldRender() && model->GetModelAsset().GetAsModel()->GetModelData()->myshaderTypeFlags & ShaderFlags::HasBones)
		{
			std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& matrixes = myBoneBuffer[boneIndexOffset];
			for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
			{
				matrixes[i] = CommonUtilities::Matrix4x4<float>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
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
	if (ImGui::TreeNode("Perlin"))
	{
		static CommonUtilities::Vector2<float> scale = { 1,1 };
		static CommonUtilities::Vector2<size_t> size = { 256,256 };
		static int seed = 0;
		bool update = false;
		update |= ImGui::SliderFloat("XScale", &scale.x, 0.01f, 200.f);
		update |= ImGui::SliderFloat("YScale", &scale.y, 0.01f, 200.f);
		update |= ImGui::InputInt("Seed", &seed);

		if (ImGui::Button("X+"))
		{
			if (size.x < 1 << 13)
			{
				size.x *= 2;
				update |= true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("X-"))
		{
			if (size.x > 64)
			{
				size.x /= 2;
				update |= true;
			}
		}
		ImGui::SameLine();
		ImGui::Text(("width: " + std::to_string(size.x)).c_str());
		if (ImGui::Button("Y+"))
		{
			if (size.y < 1 << 13)
			{
				size.y *= 2;
				update |= true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Y-"))
		{
			if (size.y > 64)
			{
				size.y /= 2;
				update |= true;
			}
		}
		ImGui::SameLine();
		ImGui::Text(("height: " + std::to_string(size.y)).c_str());
		if (update)
		{
			Texture* newperlin = GeneratePerlin(myFrameworkPtr->GetDevice(), size, scale, seed);
			if (newperlin)
			{
				myPerlinView->Release();
				myPerlinView = newperlin;
			}
		}
		Tools::ZoomableImGuiImage(myPerlinView->operator ID3D11ShaderResourceView *(),ImVec2(300,300));
		ImGui::TreePop();
	}
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
	myHighlightRenderer.Render(aModelsToHighlight, aCamera, myBoneOffsetMap);

	FullscreenPass({ Textures::Selection }, Textures::SelectionScaleDown1, FullscreenRenderer::Shader::COPY);
	FullscreenPass({ Textures::SelectionScaleDown1 }, Textures::SelEdgesHalf, FullscreenRenderer::Shader::GAUSSIANVERTICAL);
	FullscreenPass({ Textures::SelEdgesHalf }, Textures::SelEdges, FullscreenRenderer::Shader::GAUSSIANHORIZONTAL);
	FullscreenPass({ Textures::SelEdges }, Textures::SelectionScaleDown1, FullscreenRenderer::Shader::COPY);
	FullscreenPass({ Textures::Selection, Textures::SelectionScaleDown1 }, Textures::Selection2, FullscreenRenderer::Shader::MERGE);

	myStateManerger.SetBlendState(RenderStateManager::BlendState::AlphaBlend);
	FullscreenPass({ Textures::Selection2 }, Textures::BackBuffer, FullscreenRenderer::Shader::DiscardFull);
	myStateManerger.SetBlendState(RenderStateManager::BlendState::Disable);
}

bool RenderManager::CreateTextures(const unsigned int aWidth, const unsigned int aHeight)
{
	ID3D11Texture2D* backBufferTexture = myFrameworkPtr->GetBackbufferTexture();
	if (!backBufferTexture)
	{
		SYSERROR("Could not get back buffer texture", "");
		return false;
	}

	myTextures[static_cast<int>(Textures::BackBuffer)] = myFullscreenFactory.CreateTexture(backBufferTexture);
	myTextures[static_cast<int>(Textures::IntermediateDepth)] = myFullscreenFactory.CreateDepth({ aWidth, aHeight },"Main Depth");

	myTextures[static_cast<int>(Textures::IntermediateTexture)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8G8B8A8_UNORM,"Intermediate texture");
	myTextures[static_cast<int>(Textures::HalfSize)] = myFullscreenFactory.CreateTexture({ aWidth / 2U, aHeight / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM,"bloom 1/2 size");
	myTextures[static_cast<int>(Textures::QuaterSize)] = myFullscreenFactory.CreateTexture({ aWidth / 4U, aHeight / 4U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/4 size");
	myTextures[static_cast<int>(Textures::HalfQuaterSize)] = myFullscreenFactory.CreateTexture({ aWidth / 8U, aHeight / 8U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom 1/8 size");

	myTextures[static_cast<int>(Textures::SSAOBuffer)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8_UNORM,"SSAO buffer");
	myTextures[static_cast<int>(Textures::BackFaceBuffer)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8G8B8A8_UNORM,"Backface buffer");

#if ENABLEBLOOM
	myTextures[static_cast<int>(Textures::Guassian1)] = myFullscreenFactory.CreateTexture({ aWidth / 8U, aHeight / 8U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom guassian 1");
	myTextures[static_cast<int>(Textures::Guassian2)] = myFullscreenFactory.CreateTexture({ aWidth / 8U, aHeight / 8U }, DXGI_FORMAT_R8G8B8A8_UNORM, "bloom guassian 2");

	myTextures[static_cast<int>(Textures::Luminance)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8G8B8A8_UNORM, "luminance");
#endif

	myTextures[static_cast<int>(Textures::SelectionScaleDown1)] = myFullscreenFactory.CreateTexture({ aWidth / 2U, aHeight / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 1/2");
	myTextures[static_cast<int>(Textures::SelectionScaleDown2)] = myFullscreenFactory.CreateTexture({ aWidth / 4U, aHeight / 4U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 1/4");

	myTextures[static_cast<int>(Textures::Selection)] = myFullscreenFactory.CreateTexture({ aWidth / 2U, aHeight / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection");
	myTextures[static_cast<int>(Textures::SelEdgesHalf)] = myFullscreenFactory.CreateTexture({ aWidth / 2U, aHeight / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection edges half");
	myTextures[static_cast<int>(Textures::SelEdges)] = myFullscreenFactory.CreateTexture({ aWidth / 2U, aHeight / 2U }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection edges all");
	myTextures[static_cast<int>(Textures::Selection2)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8G8B8A8_UNORM, "selection 2");

	myTextures[static_cast<int>(Textures::Edges)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8_UNORM, "Edges");
	myTextures[static_cast<int>(Textures::AAHorizontal)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8G8B8A8_UNORM, "AA horisontal");

	myTextures[static_cast<int>(Textures::LUT)] = myFullscreenFactory.CreateTexture({ aWidth, aHeight }, DXGI_FORMAT_R8G8B8A8_UNORM, "LUT buffer");

	myRandomNormalview = LoadTexture(myFrameworkPtr->GetDevice(), "Data/Textures/SSAONormal.dds");
	myGBuffer = myFullscreenFactory.CreateGBuffer({ aWidth,aHeight }, "main gBuffer");
	myBufferGBuffer = myFullscreenFactory.CreateGBuffer({ aWidth,aHeight }, "secondary GBuffer");

	for (auto& i : myTextures)
	{
		if (!i)
		{
			SYSERROR("Could not initialize all fullscreen textures", "");
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

	DirectX11Framework::AddMemoryUsage(sizeof(BoneTextureCPUBuffer) * DirectX11Framework::FormatToSizeLookup[desc.Format], "Bone Texture", "Engine Texture");


	HRESULT result = device->CreateTexture2D(&desc, nullptr, &myBoneBufferTexture);
	if (FAILED(result))
	{
		SYSERROR("Could not create bone buffer texture", "");
		return false;
	}


	result = device->CreateShaderResourceView(myBoneBufferTexture, nullptr, &myBoneTextureView);
	if (FAILED(result))
	{
		SYSERROR("Could not create bone shader resource view", "");
		return false;
	}

	return true;
}

void RenderManager::Resize(const unsigned int aWidth, const unsigned int aHeight)
{
	if (myScreenSize.x != aWidth || myScreenSize.y != aHeight)
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
				SYSERROR("Could not resize swap chain buffer.", "");
			}
		}

		CreateTextures(aWidth, aHeight);
		myTextures[static_cast<int>(Textures::BackBuffer)].SetAsActiveTarget();

		SetScreendumpTexture(myFrameworkPtr->GetContext(), myFrameworkPtr->GetBackbufferTexture());

		myScreenSize = { aWidth,  aHeight };
	}
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

