#ifndef ENGINE_GRAPHICS_GBUFFER_H
#define ENGINE_GRAPHICS_GBUFFER_H

#include "engine/graphics/Texture.h"

#include "tools/MathVector.h"

#include <array>

#include <d3d11.h>
namespace engine
{
	class GBuffer
	{
	public:
		enum class Textures
		{
			Postion,
			Albedo,
			Normal,
			VertexNormal,
			Metalness,
			Roughness,
			AmbientOcclusion,
			Emissive,
			Count
		};

		void ClearTextures(const tools::V4f aClearColor = tools::V4f(0.f, 0.f, 0.f, 0.f));
		void SetAsActiveTarget(class Texture* aDepth = nullptr);
		void SetAsActiveTarget(const Textures aResource, class Texture* aDepth = nullptr);
		void SetAsResourceOnSlot(const Textures aResource, const unsigned int aSlot);
		void SetAllAsResources();
		void CopyTo(GBuffer* aOther, ID3D11DeviceContext* aContext);

		void Release();

	private:
		friend class TextureFactory;

		std::array<ID3D11Texture2D*, static_cast<int>(Textures::Count)> myTextures;
		std::array<ID3D11RenderTargetView*, static_cast<int>(Textures::Count)> myRenderTargets;
		std::array<ID3D11ShaderResourceView*, static_cast<int>(Textures::Count)> myShaderResources;

		D3D11_VIEWPORT* myViewport;
		ID3D11DeviceContext* myContext;
	};
}
#endif