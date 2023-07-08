#ifndef ENGINE_GRAPHICS_FULLSCREEN_RENDERER_H
#define ENGINE_GRAPHICS_FULLSCREEN_RENDERER_H

#include "engine/assets/Asset.h"

#include <array>

namespace engine::graphics
{
	class FullscreenRenderer
	{
	public:
		enum class Shader
		{
			COPY,
			LUMINANCE,
			GAUSSIANHORIZONTAL,
			GAUSSIANVERTICAL,
			MERGE,
			DeferedToToon,
			DiscardFull,
			SSAO,
			EdgeDetection,
			ConditionalGAUSSIANHORIZONTAL,
			ConditionalGAUSSIANVERTICAL,
			COUNT
		};

	public:
		FullscreenRenderer();
		~FullscreenRenderer() = default;

		bool Init();

		void Render(Shader aEffect);
		void Render(const AssetHandle<PixelShaderAsset>& aShader);

	private:
		AssetHandle<VertexShaderAsset> myVertexShader = nullptr;
		std::array<AssetHandle<PixelShaderAsset>, static_cast<int>(Shader::COUNT)> myPixelShaders;

	};

}

#endif