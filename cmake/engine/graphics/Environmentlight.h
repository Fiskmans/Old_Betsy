#ifndef ENGINE_GRAPHICS_ENVIRONMENT_LIGHT_H
#define ENGINE_GRAPHICS_ENVIRONMENT_LIGHT_H

#include "engine/assets/Asset.h"

#include "engine/graphics/Camera.h"

#include "tools/MathVector.h"

namespace engine::graphics
{
	struct EnvironmentLight
	{
		AssetHandle<TextureAsset> myTexture;
		tools::V3f myDirection;
		tools::V3f myColor;
		tools::V3f myCenter;
		float myIntensity;
	};
}

#endif // ENGINE_GRAPHICS_ENVIRONMENT_LIGHT_H