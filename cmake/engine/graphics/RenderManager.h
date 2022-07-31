#ifndef ENGINE_GRAPHICS_RENDER_MANAGER_H
#define ENGINE_GRAPHICS_RENDER_MANAGER_H

#include "engine/graphics/WindowManager.h"
#include "engine/graphics/Texture.h"
#include "engine/graphics/GBuffer.h"
#include "engine/graphics/FullscreenRenderer.h"
#include "engine/graphics/RenderStateManager.h"
#include "engine/graphics/BoneBuffer.h"
#include "engine/graphics/DeferredRenderer.h"

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
//#include "HighlightRenderer.h"
//#include "DepthRenderer.h"

//#include "GBuffer.h"
//#include "TextRenderer.h"

namespace engine::graphics
{

	class RenderManager
		: public tools::Singleton<RenderManager>
	{
	public:
		class TextureMapping
		{
		public:
			TextureMapping(const AssetHandle& aHandle, size_t aIndex);

		private:
			friend RenderManager;
			ID3D11ShaderResourceView* myResource;
			size_t mySlot;
		};

		RenderManager();
		~RenderManager() = default;

		bool Init();
		void Resize(const tools::V2ui& aSize);

		void BeginFrame(tools::V4f aClearColor);
		void EndFrame();

		void Render();

		void MapTextures(AssetHandle& aTarget, const std::vector<TextureMapping>& aTextures, DepthTexture* aDepth = nullptr);
		void MapTextures(GBuffer& aTarget, const std::vector<TextureMapping>& aTextures, DepthTexture* aDepth = nullptr);

		void Imgui();
		bool myDoDebugLines = true;

		RenderStateManager& GetRenderStateManager() { return myRenderStateManager; }

		FullscreenRenderer& GetFullscreenRender() { return myFullscreenRenderer; }
		DeferredRenderer& GetDeferredRenderer() { return myDeferredRenderer; }

		template<class BufferType>
		static bool CreateGenericShaderBuffer(ID3D11Buffer*& aBuffer) { return CreateGenericShaderBuffer(aBuffer, sizeof(BufferType)); }
		static bool CreateGenericShaderBuffer(ID3D11Buffer*& aBuffer, size_t aSize);

		static bool OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize);
	private:
		float myStartedAt = 0.f;
		bool myIsReady = false;;

		RenderStateManager myRenderStateManager;

		FullscreenRenderer myFullscreenRenderer;
		DeferredRenderer myDeferredRenderer;
		//ForwardRenderer myForwardRenderer;
		//SpriteRenderer mySpriteRenderer;
		//ParticleRenderer myParticleRenderer;
		//TextRenderer myTextRenderer;
		//HighlightRenderer myHighlightRenderer;
		//DepthRenderer myShadowRenderer;
		//AssetHandle myPerlinView;
		//AssetHandle myRandomNormal;

		enum class Channel
		{
			BackBuffer,
			IntermediateTexture,
			Count
		};
		std::array<Texture, static_cast<int>(Channel::Count)> myTextures;
		BoneTextureCPUBuffer myBoneBuffer;

#if ENABLESSAO
		bool myDoSSAO = false;
#endif
		bool myDoAA = true;
		bool myShouldRenderWireFrame = false;

		void SetupBoneTexture(const std::vector<ModelInstance*>& aModelList);
		//void RenderSelection(const std::vector<ModelInstance*>& aModelsToHighlight, Camera* aCamera);
		bool CreateTextures(const tools::V2ui& aSize);

		void FullscreenPass(std::vector<Channel> aSources, Channel aTarget, FullscreenRenderer::Shader aShader);
		void UnbindResources();
		void UnbindTargets();
	};

}

#endif