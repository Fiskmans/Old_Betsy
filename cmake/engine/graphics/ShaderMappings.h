#ifndef ENGINE_GRAPHICS_SHADER_MAPPINGS
#define ENGINE_GRAPHICS_SHADER_MAPPINGS

namespace engine::graphics::shader_mappings
{
	constexpr size_t CONSTANT_BUFFER_ENV_LIGHT = 0;

	constexpr size_t TEXTURE_ALBEDO = 0;
	constexpr size_t TEXTURE_NORMAL = 1;
	constexpr size_t TEXTURE_MATERIAL = 2;

	constexpr size_t TEXTURE_SKYBOX = 3;
	constexpr size_t TEXTURE_ENVIRONMENT = 4;

	constexpr size_t TEXTURE_PERLIN = 5;

}

#endif