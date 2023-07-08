#ifndef ENGINE_GRAPHICS_DEPTH_RENDERER_H
#define ENGINE_GRAPHICS_DEPTH_RENDERER_H

#include "engine/graphics/Camera.h"

#include "engine/assets/PointLight.h"

#include <array>
#include <memory>

namespace engine::graphics
{

	class DepthRenderer
	{
	public:
		~DepthRenderer();

		const std::array<Camera*, 6>& GetCameras();
		const Camera* GetEnvirontmentCamera();
		void BindshadowsToSlots(int aSlot);
		void BindShadowDensityToSlot(int aSlot);
		void Render(Camera* aSpotlight, BoneTextureCPUBuffer& aBoneBuffer);

	private:

		void Render(Camera* aCamera, const std::vector<ModelInstance*>& aModelList, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping);
		struct ID3D11Buffer* myFrameBuffer = nullptr;
		struct ID3D11Buffer* myObjectBuffer = nullptr;

		AssetHandle<VertexShaderAsset> myShader;
		Camera* myEnvironmentCamera = nullptr;

		ID3D11ShaderResourceView* myLastDepthView = { nullptr };

		// 1x1
		ID3D11DepthStencilView* myDepth1x1 = nullptr;
		ID3D11ShaderResourceView* myDepthsResource1x1 = nullptr;
		ID3D11RenderTargetView* myRenderTarget1x1 = nullptr;
		ID3D11ShaderResourceView* myRenderTargetResourceView1x1 = nullptr;

		ID3D11RenderTargetView* myDecalRenderTarget = nullptr;
	};

}

#endif // 