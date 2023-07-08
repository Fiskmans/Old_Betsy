#ifndef ENGINE_GRAPHICS_RENDER_STATE_MANAGER_H
#define ENGINE_GRAPHICS_RENDER_STATE_MANAGER_H

#include <array>

#define NOMINMAX
#include <WinSock2.h>
#include <d3d11.h>

namespace engine::graphics
{
	class RenderStateManager
	{
	public:
		enum class BlendState
		{
			Disable,
			AlphaBlend,
			AdditativeBlend,
			Count
		};
		enum class DepthStencilState
		{
			Default,
			ReadOnly,
			OnlyCovered,
			Count
		};
		enum class RasterizerState
		{
			Default,
			Wireframe,
			NoBackfaceCulling,
			CullFrontFacing,
			Count
		};
		enum class SamplerState
		{
			Trilinear,
			Point,
			Count
		};

	public:

		bool Init();

		void SetBlendState(BlendState aState);
		void SetDepthStencilState(DepthStencilState aState);
		void SetRasterizerState(RasterizerState aState);
		void SetSamplerState(SamplerState aState);

		void SetAllStates(BlendState aBlendState = BlendState::Disable, DepthStencilState aDepthState = DepthStencilState::Default, RasterizerState aRasterizerState = RasterizerState::Default, SamplerState aSamplerState = SamplerState::Trilinear);

	private:
		bool CreateBlendStates();
		bool CreateDepthStencilStates();
		bool CreateRasterizerStates();
		bool CreateSamplerStates();

		std::array<ID3D11BlendState*, static_cast<int>(BlendState::Count)> myBlendStates;
		std::array<ID3D11DepthStencilState*, static_cast<int>(DepthStencilState::Count)> myDepthStencilStates;
		std::array<ID3D11RasterizerState*, static_cast<int>(RasterizerState::Count)> myRasterizerStates;
		std::array<ID3D11SamplerState*, static_cast<int>(SamplerState::Count)> mySamplerStates;
	};

}

#endif