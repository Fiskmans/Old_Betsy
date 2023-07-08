#ifndef ENGINE_GRAPHICS_SHADER_MAPPINGS
#define ENGINE_GRAPHICS_SHADER_MAPPINGS

namespace engine::graphics::shader_mappings
{
	constexpr size_t BUFFER_FRAME = 0;
	constexpr size_t BUFFER_OBJECT = 1;
	constexpr size_t BUFFER_POINT_LIGHT = 2;
	constexpr size_t BUFFER_ANIMATION = 3;
	constexpr size_t BUFFER_DEFFERED_FRAME = 4;
	constexpr size_t BUFFER_DEFFERED_CLOUDS = 5;


	constexpr size_t TEXTURE_POSITION = 0;
	constexpr size_t TEXTURE_ALBEDO = 1;
	constexpr size_t TEXTURE_NORMAL = 2;
	constexpr size_t TEXTURE_VERTEXNORMAL = 3;
	constexpr size_t TEXTURE_MATERIAL = 4;

	constexpr size_t TEXTURE_SKYBOX = 5;
	constexpr size_t TEXTURE_ENVIRONMENT = 6;
	constexpr size_t TEXTURE_SHADOWS = 7;

	constexpr size_t TEXTURE_PERLIN = 8;
	constexpr size_t TEXTURE_DEPTH = 9;

}

#endif