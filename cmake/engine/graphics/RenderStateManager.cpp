
#include "engine/graphics/RenderStateManager.h"
#include "engine/graphics/GraphicEngine.h"

#include "tools/Logger.h"

#include "common/Macros.h"

namespace engine::graphics
{
	bool RenderStateManager::Init()
	{
		if (!CreateBlendStates())
		{
			LOG_SYS_ERROR("Could not create blend states!");
			return false;
		}
		if (!CreateDepthStencilStates())
		{
			LOG_SYS_ERROR("Could not create depth stencil states!");
			return false;
		}
		if (!CreateRasterizerStates())
		{
			LOG_SYS_ERROR("Could not create rasterizer states!");
			return false;
		}
		if (!CreateSamplerStates())
		{
			LOG_SYS_ERROR("Could not create sampler states!");
			return false;
		}

		return true;
	}

	void RenderStateManager::SetBlendState(BlendState aState)
	{
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->OMSetBlendState(myBlendStates[static_cast<int>(aState)], nullptr, D3D11_DEFAULT_SAMPLE_MASK);
	}

	void RenderStateManager::SetDepthStencilState(DepthStencilState aState)
	{
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->OMSetDepthStencilState(myDepthStencilStates[static_cast<int>(aState)], 0);
	}

	void RenderStateManager::SetRasterizerState(RasterizerState aState)
	{
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->RSSetState(myRasterizerStates[static_cast<int>(aState)]);
	}

	void RenderStateManager::SetSamplerState(SamplerState aState)
	{
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->PSSetSamplers(0, 1, &mySamplerStates[static_cast<int>(aState)]);
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->VSSetSamplers(0, 1, &mySamplerStates[static_cast<int>(aState)]);
	}

	void RenderStateManager::SetAllStates(BlendState aBlendState, DepthStencilState aDepthState, RasterizerState aRasterizerState, SamplerState aSamplerState)
	{
		SetBlendState(aBlendState);
		SetDepthStencilState(aDepthState);
		SetRasterizerState(aRasterizerState);
		SetSamplerState(aSamplerState);
	}

	bool RenderStateManager::CreateBlendStates()
	{
		ID3D11Device* device = GraphicsEngine::GetInstance().GetFrameWork().GetDevice();

		HRESULT result;

		D3D11_BLEND_DESC alphaDesc;
		WIPE(alphaDesc);
		alphaDesc.RenderTarget[0].BlendEnable = true;
		alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		CD3D11_BLEND_DESC addDesc;
		WIPE(addDesc);
		addDesc.RenderTarget[0].BlendEnable = true;
		addDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		addDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		addDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		addDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		addDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		addDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		addDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = device->CreateBlendState(&alphaDesc, &myBlendStates[static_cast<int>(BlendState::AlphaBlend)]);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create alpha blend state.");
			return false;
		}

		result = device->CreateBlendState(&addDesc, &myBlendStates[static_cast<int>(BlendState::AdditativeBlend)]);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create additative blend state.");
			return false;
		}

		myBlendStates[static_cast<int>(BlendState::Disable)] = nullptr;

		return true;
	}

	bool RenderStateManager::CreateDepthStencilStates()
	{
		ID3D11Device* device = GraphicsEngine::GetInstance().GetFrameWork().GetDevice();

		HRESULT result;

		{
			D3D11_DEPTH_STENCIL_DESC desc;
			WIPE(desc);
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.StencilEnable = false;

			result = device->CreateDepthStencilState(&desc, &myDepthStencilStates[static_cast<int>(DepthStencilState::ReadOnly)]);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Failed to create depth stencil state.");
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_DESC desc;
			WIPE(desc);
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.StencilEnable = false;

			result = device->CreateDepthStencilState(&desc, &myDepthStencilStates[static_cast<int>(DepthStencilState::Default)]);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Failed to create depth stencil state.");
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_DESC desc;
			WIPE(desc);
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
			desc.StencilEnable = false;

			result = device->CreateDepthStencilState(&desc, &myDepthStencilStates[static_cast<int>(DepthStencilState::OnlyCovered)]);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Failed to create depth stencil state.");
				return false;
			}
		}

		return true;
	}

	bool RenderStateManager::CreateRasterizerStates()
	{
		ID3D11Device* device = GraphicsEngine::GetInstance().GetFrameWork().GetDevice();

		HRESULT result;

		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthClipEnable = true;

		result = device->CreateRasterizerState(&desc, &myRasterizerStates[static_cast<int>(RasterizerState::Wireframe)]);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create rasterizer wireframe state.");
			return false;
		}

		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthClipEnable = true;

		result = device->CreateRasterizerState(&desc, &myRasterizerStates[static_cast<int>(RasterizerState::NoBackfaceCulling)]);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create rasterizer noBackfaceCulling state.");
			return false;
		}


		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.DepthClipEnable = true;

		result = device->CreateRasterizerState(&desc, &myRasterizerStates[static_cast<int>(RasterizerState::CullFrontFacing)]);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create rasterizer frontFaceculling state.");
			return false;
		}

		myRasterizerStates[static_cast<int>(RasterizerState::Default)] = nullptr;

		return true;
	}

	bool RenderStateManager::CreateSamplerStates()
	{
		ID3D11Device* device = GraphicsEngine::GetInstance().GetFrameWork().GetDevice();

		HRESULT result;

		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		result = device->CreateSamplerState(&desc, &mySamplerStates[static_cast<int>(SamplerState::Point)]);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create point sampler state.");
			return false;
		}

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;


		result = device->CreateSamplerState(&desc, &mySamplerStates[static_cast<int>(SamplerState::Trilinear)]);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create point sampler state.");
			return false;
		}

		return true;
	}
}