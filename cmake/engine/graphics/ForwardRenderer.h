#ifndef ENGINE_GRAPHICS_FORWARD_RENDERER_H
#define ENGINE_GRAPHICS_FORWARD_RENDERER_H

#include "engine/assets/ModelInstance.h"

#include "engine/graphics/Camera.h"

#include <vector>

namespace engine::graphics
{
	class ForwardRenderer
	{
	public:
		bool Init();

		void Render(std::vector<ModelInstance*>& aModels, Camera& aCamera);

	private:
		ID3D11Buffer* myObjectBuffer;

		AssetHandle<PixelShaderAsset> myPixelShader;
		AssetHandle<VertexShaderAsset> myVertexShader;
	};
}

#endif 