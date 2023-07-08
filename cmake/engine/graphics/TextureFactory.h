#ifndef ENGINE_GRAPHICS_TEXTURE_FACTORY_H
#define ENGINE_GRAPHICS_TEXTURE_FACTORY_H

#include "engine/graphics/GraphicsFramework.h"
#include "engine/graphics/Texture.h"
#include "engine/graphics/GBuffer.h"

#include "tools/MathVector.h"
#include "tools/Singleton.h"

#include <d3d11.h>

namespace engine::graphics
{
	class TextureFactory
		: public fisk::tools::Singleton<TextureFactory>
	{
	public:
		TextureFactory() = default;
		~TextureFactory() = default;

		UpdatableTexture* CreateUpdatableTexture(tools::V2ui aSize, const std::string& aName);
		Texture CreateTexture(tools::V2ui aSize, DXGI_FORMAT aFormat, const std::string& aName);
		void CreateTexture(ID3D11Texture2D* aTexture, Texture& aTextureObject);
		DepthTexture CreateDepth(tools::V2ui aSize, const std::string& aName);
		GBuffer CreateGBuffer(const tools::V2ui& aSize, const std::string& aName);
	};

}

#endif