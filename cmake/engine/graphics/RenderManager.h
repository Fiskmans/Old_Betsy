#ifndef ENGINE_GRAPHICS_RENDER_MANAGER_H
#define ENGINE_GRAPHICS_RENDER_MANAGER_H

#include "engine/graphics/BoneBuffer.h"
#include "engine/graphics/Camera.h"
#include "engine/graphics/FullscreenRenderer.h"
#include "engine/graphics/GBuffer.h"
#include "engine/graphics/RenderStateManager.h"
#include "engine/graphics/Texture.h"
#include "engine/graphics/WindowManager.h"

#include "engine/assets/ModelInstance.h"

#include "engine/ImGuiWindow.h"

#include "tools/Singleton.h"
#include "tools/MathVector.h"

#include <array>
#include <vector>
#include <unordered_map>

#include <d3d11.h>

namespace engine::graphics
{

	class RenderManager : 
		public fisk::tools::Singleton<RenderManager>,
		public engine::ImGuiWindow
	{
	public:
		class TextureMapping
		{
		public:
			TextureMapping(const AssetHandle<TextureAsset>& aHandle, size_t aIndex);

		private:
			friend RenderManager;
			ID3D11ShaderResourceView* myResource;
			size_t mySlot;
		};

		RenderManager();
		~RenderManager() = default;

		bool Init();
		void Resize(const tools::V2ui& aSize);

		void Render();

		void RenderCamera(engine::Camera& aCamera);

		void MapTextures(AssetHandle<DrawableTextureAsset>& aTarget, const std::vector<TextureMapping>& aTextures, AssetHandle<DepthTextureAsset> aDepth = nullptr);
		void MapTextures(AssetHandle<GBufferAsset>& aTarget, const std::vector<TextureMapping>& aTextures, AssetHandle<DepthTextureAsset> aDepth = nullptr);

		RenderStateManager& GetRenderStateManager() { return myRenderStateManager; }

		template<class BufferType>
		static bool CreateGenericShaderBuffer(ID3D11Buffer*& aBuffer) { return CreateGenericShaderBuffer(aBuffer, sizeof(BufferType)); }
		static bool CreateGenericShaderBuffer(ID3D11Buffer*& aBuffer, size_t aSize);

		static bool OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize);
		static bool SetShaderResource(size_t aSlot, AssetHandle<TextureAsset>& aAsset);

		void OnImgui() override;
		inline const char* ImGuiName() override { return "Rendering"; };

	private:

		bool myDoDebugLines = true;
		bool myStaticClearColor = false;
		tools::V4f myClearColor = tools::V4f(1,1,0,1);


		fisk::tools::EventReg myResolutionChangedEventHandle;
	
		RenderStateManager myRenderStateManager;

		enum class Channel
		{
			BackBuffer,
			IntermediateTexture,
			Count
		};
		std::array<Texture, static_cast<int>(Channel::Count)> myTextures;
		BoneTextureCPUBuffer myBoneBuffer;
		DepthTexture myDepthTexture;
		GBuffer myGBuffer;

		FullscreenRenderer myFullscreenRenderer;

		bool CreateTextures(const tools::V2ui& aSize);

		void FullscreenPass(std::vector<Channel> aSources, Channel aTarget, FullscreenRenderer::Shader aShader);
		void UnbindResources();
		void UnbindTargets();
	};

}

#endif