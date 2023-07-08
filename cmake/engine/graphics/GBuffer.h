#ifndef ENGINE_GRAPHICS_GBUFFER_H
#define ENGINE_GRAPHICS_GBUFFER_H

#include "engine/graphics/Texture.h"

#include "tools/MathVector.h"

#include "imgui/imgui.h"

#include <array>

#define NOMINMAX
#include <WinSock2.h>
#include <d3d11.h>

namespace engine::graphics
{
	class GBuffer
	{
	public:
		GBuffer() = default;
		GBuffer(const GBuffer&) = delete;
		void operator=(const GBuffer&) = delete;

		~GBuffer();
		GBuffer(GBuffer&&);
		void operator=(GBuffer&&);

		enum class Channel
		{
			Postion,
			Albedo,
			Normal,
			VertexNormal,
			Material,
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

	private:
		friend class TextureFactory;

		std::array<ID3D11Texture2D*, static_cast<int>(Channel::Count)> myTextures = { nullptr };
		std::array<ID3D11RenderTargetView*, static_cast<int>(Channel::Count)> myRenderTargets = { nullptr };
		std::array<ID3D11ShaderResourceView*, static_cast<int>(Channel::Count)> myShaderResources = { nullptr };

		D3D11_VIEWPORT* myViewport = nullptr;
	};
}
#endif