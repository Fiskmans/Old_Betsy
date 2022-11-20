#ifndef ENGINE_GRAPHICS_DEFERRED_RENDERER_H
#define ENGINE_GRAPHICS_DEFERRED_RENDERER_H

#include "engine/assets/ModelInstance.h"

#include "engine/graphics/Camera.h"
#include "engine/graphics/Environmentlight.h"

#include <vector>

namespace engine::graphics
{
	class DeferredRenderer
	{
	public:
		struct FilterResult
		{
			std::vector<ModelInstance*> myDeferrable;
			std::vector<ModelInstance*> myRest;
		};

		DeferredRenderer() = default;
		~DeferredRenderer() = default;

		bool Init();


		FilterResult Filter(const std::vector<ModelInstance*>& aModels);

		void GenerateGBuffer(Camera* aCamera);

		void Render(Camera* aCamera);

	private:
		void MapEnvLightBuffer(Camera* aCamera);

		AssetHandle<PixelShaderAsset> myBackFaceShader;
		ID3D11Buffer* myFrameBuffer = nullptr;
		ID3D11Buffer* myObjectBuffer = nullptr;
		ID3D11Buffer* myPixelEnvLightBuffer = nullptr;
		ID3D11Buffer* myPixelPointLightBuffer = nullptr;
		ID3D11Buffer* myPixelSpotLightBuffer = nullptr;
		ID3D11Buffer* myDecalBuffer = nullptr;
	};
}

#endif