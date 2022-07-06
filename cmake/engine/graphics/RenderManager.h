#ifndef ENGINE_GRAPHICS_RENDER_MANAGER_H
#define ENGINE_GRAPHICS_RENDER_MANAGER_H

#include "engine/graphics/WindowManager.h"
#include "engine/graphics/Texture.h"
#include "engine/graphics/GBuffer.h"
#include "engine/graphics/FullscreenRenderer.h"
#include "engine/graphics/RenderStateManager.h"
#include "engine/graphics/BoneBuffer.h"

#include "engine/assets/ModelInstance.h"

#include "tools/Singleton.h"
#include "tools/MathVector.h"

#include <array>
#include <vector>
#include <unordered_map>

#include <d3d11.h>

//#include "TextureFactory.h"
//#include "Texture.h"

//#include "ForwardRenderer.h"
//#include "FullscreenRenderer.h"
//#include "SpriteRenderer.h"
//#include "ParticleRenderer.h"
//#include "DeferredRenderer.h"
//#include "HighlightRenderer.h"
//#include "DepthRenderer.h"

//#include "GBuffer.h"
//#include "TextRenderer.h"

namespace engine
{

	class RenderManager
		: public tools::Singleton<RenderManager>
	{
	public:
		RenderManager();
		~RenderManager() = default;

		bool Init();
		void Resize(const tools::V2ui& aSize);

		void BeginFrame(tools::V4f aClearColor);
		void EndFrame();

		void Render();


		void Imgui();
		bool myDoDebugLines = true;
	private:
		float myStartedAt = 0.f;
		bool myIsReady = false;;

		std::unordered_map<ModelInstance*, short> myBoneOffsetMap;

		RenderStateManager myStateManager;

		//ForwardRenderer myForwardRenderer;
		FullscreenRenderer myFullscreenRenderer;
		//SpriteRenderer mySpriteRenderer;
		//ParticleRenderer myParticleRenderer;
		//DeferredRenderer myDeferredRenderer;
		//TextRenderer myTextRenderer;
		//HighlightRenderer myHighlightRenderer;
		//DepthRenderer myShadowRenderer;
		//AssetHandle myPerlinView;
		//AssetHandle myRandomNormal;

		ID3D11Texture2D* myBoneBufferTexture = nullptr;
		ID3D11ShaderResourceView* myBoneTextureView = nullptr;

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
		bool myShouldRenderWireFrame = false;

		void SetupBoneTexture(const std::vector<ModelInstance*>& aModelList);
		//void RenderSelection(const std::vector<ModelInstance*>& aModelsToHighlight, Camera* aCamera);
		bool CreateTextures(const tools::V2ui& aSize);

		void FullscreenPass(std::vector<Textures> aSources, Textures aTarget, FullscreenRenderer::Shader aShader);
		void UnbindResources();
		void UnbindTargets();
	};

}

#endif