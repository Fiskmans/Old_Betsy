#pragma once
#include "TextureFactory.h"
#include "Texture.h"

#include "ForwardRenderer.h"
#include "FullscreenRenderer.h"
#include "SpriteRenderer.h"
#include "ParticleRenderer.h"
#include "DeferredRenderer.h"
#include "HighlightRenderer.h"
#include "DepthRenderer.h"

#include "Observer.hpp"
#include "RenderStateManager.h"
#include "GBuffer.h"
#include "TextRenderer.h"

#include "GamlaBettan\DirectX11Framework.h"
#include "GamlaBettan\WindowHandler.h"

class SpriteRenderer;
struct ID3D11ShaderResourceView;

class RenderManager
	: public Observer
	, public CommonUtilities::Singleton<RenderManager>
{
public:
	RenderManager();
	~RenderManager() = default;

	bool Init(DirectX11Framework* aFramework);
	bool Release();

	void BeginFrame(float aClearColor[4]);
	void EndFrame();

	void Render();
	void RenderSprites(const std::vector<SpriteInstance*>& aSpriteList);
	void RenderText(const std::vector<TextInstance*>& aTextList);

	void AddExtraSpriteToRender(SpriteInstance* aSprite);

	void RecieveMessage(const Message& aMessage);

	void SetupBoneTexture(const std::vector<ModelInstance*>& aModelList);

	SpriteRenderer* GetSpriteRenderer();

	static float GetTotalTime();

#if USEIMGUI
	void Imgui();
#endif // !_RETAIL
#if USEDEBUGLINES
	bool myDoDebugLines = true;
#endif
private:

	std::vector<SpriteInstance*> myExtraSpritesToRenderThisFrame;
	std::unordered_map<ModelInstance*, short> myBoneOffsetMap;

	float myStartedAt;
	static float ourTotalTime;
	RenderStateManager myStateManerger;
	ForwardRenderer myForwardRenderer;
	FullscreenRenderer myFullscreenRenderer;
	SpriteRenderer mySpriteRenderer;
	ParticleRenderer myParticleRenderer;
	DeferredRenderer myDeferredRenderer;
	TextRenderer myTextRenderer;
	HighlightRenderer myHighlightRenderer;
	DepthRenderer myShadowRenderer;
	DirectX11Framework* myFrameworkPtr;
	ID3D11ShaderResourceView* myBoneTextureView = nullptr;
	AssetHandle myPerlinView;
	AssetHandle	myRandomNormal;

	struct ID3D11Texture2D* myBoneBufferTexture = nullptr;

	enum class Textures
	{
		BackBuffer,
		IntermediateTexture,
		HalfSize,
		QuaterSize,
		HalfQuaterSize,
		IntermediateDepth,
#if ENABLEBLOOM
		Guassian1,
		Guassian2,
		Luminance,
#endif
		Selection,
		SelEdgesHalf,
		SelEdges,
		Selection2,
		SelectionScaleDown1,
		SelectionScaleDown2,
		SSAOBuffer,
		BackFaceBuffer,
		Edges,
		AAHorizontal,
		LUT,
		Count
	};
	std::array<Texture, static_cast<int>(Textures::Count)> myTextures;
	GBuffer myGBuffer;
	GBuffer myBufferGBuffer;
	BoneTextureCPUBuffer myBoneBuffer;

#if ENABLESSAO
	bool myDoSSAO = false;
#endif
	bool myDoAA = true;
#if USEIMGUI
	bool myShouldRenderWireFrame;
	std::array<ImVec2, static_cast<int>(Textures::Count)> lastKnownPositions;
#endif // !_RETAIL

	void RenderSelection(const std::vector<ModelInstance*>& aModelsToHighlight, Camera* aCamera);
	bool CreateTextures(const V2ui& aSize);
	void Resize(const V2ui& aSize);

	void FullscreenPass(std::vector<Textures> aSources, Textures aTarget, FullscreenRenderer::Shader aShader);
	void UnbindResources();
	void UnbindTargets();
};

