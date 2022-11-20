#ifndef ENGINE_GRAPHICS_GBUFFER_H
#define ENGINE_GRAPHICS_GBUFFER_H

#include "engine/graphics/Texture.h"

#include "tools/MathVector.h"

#include "imgui/imgui.h"

#include <array>

#include <d3d11.h>

namespace engine::graphics
{
	class GBuffer
	{
	public:
		enum class Channel
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

		bool IsValid() { return !!myViewport; };

		void ClearTextures(const tools::V4f aClearColor = tools::V4f(0.f, 0.f, 0.f, 0.f));
		void SetAsActiveTarget(const DepthTexture* aDepth = nullptr) const;
		void SetAsActiveTarget(const Channel aResource, const DepthTexture* aDepth = nullptr) const;
		void SetAsResourceOnSlot(const Channel aResource, const unsigned int aSlot);
		void SetAllAsResources() const;
		void CopyTo(GBuffer* aOther, ID3D11DeviceContext* aContext);

		void Imgui(ImVec2 aTopLeft, ImVec2 aBottomRight) const;

		void Release();

	private:
		friend class TextureFactory;

		std::array<ID3D11Texture2D*, static_cast<int>(Channel::Count)> myTextures;
		std::array<ID3D11RenderTargetView*, static_cast<int>(Channel::Count)> myRenderTargets;
		std::array<ID3D11ShaderResourceView*, static_cast<int>(Channel::Count)> myShaderResources;

		D3D11_VIEWPORT* myViewport = nullptr;
	};
}
#endif