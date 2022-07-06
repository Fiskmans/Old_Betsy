#ifndef ENGINE_GRAPHICS_FULLSCREEN_RENDERER_H
#define ENGINE_GRAPHICS_FULLSCREEN_RENDERER_H

#include "engine/assets/Asset.h"

#include <array>

namespace engine
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
			PBREnvironmentLight,
			PBRPointLight,
			DiscardFull,
			SSAO,
			PBRSpotLight,
			Cloud,
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
		void Render(const AssetHandle& aShader);

	private:
		AssetHandle myVertexShader = nullptr;
		std::array<AssetHandle, static_cast<int>(Shader::COUNT)> myPixelShaders;

	};

}

#endif