#include "engine/graphics/TextureFactory.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/GraphicsFramework.h"

#include "common/Macros.h" 

#include "tools/Logger.h"

namespace engine::graphics
{
	UpdatableTexture* TextureFactory::CreateUpdatableTexture(tools::V2ui aSize, const std::string& aName)
	{
		GraphicsFramework& framework = GraphicsEngine::GetInstance().GetFrameWork();
		ID3D11Device* device = framework.GetDevice();
		
		HRESULT result;
		
		D3D11_TEXTURE2D_DESC desc;
		WIPE(desc);
		desc.Width = aSize[0];
		desc.Height = aSize[1];
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		
		ID3D11Texture2D* texture;
		result = device->CreateTexture2D(&desc, nullptr, &texture);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create texture");
			return nullptr;
		}
		
		UpdatableTexture* returnVal = new UpdatableTexture(aSize);

		ID3D11ShaderResourceView* shaderResource;
		result = device->CreateShaderResourceView(texture, nullptr, &shaderResource);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create shader resource");
			return nullptr;
		}
		returnVal->myShaderResource = shaderResource;
		returnVal->myTexture = texture;
		returnVal->myViewport = new D3D11_VIEWPORT({ 0.f, 0.f, static_cast<float>(aSize[0]), static_cast<float>(aSize[1]), 0.f, 1.f });

		return returnVal;
	}

	Texture TextureFactory::CreateTexture(tools::V2ui aSize, DXGI_FORMAT aFormat, const std::string& aName)
	{
		ID3D11Device* device = GraphicsEngine::GetInstance().GetFrameWork().GetDevice();
		HRESULT result;

		D3D11_TEXTURE2D_DESC desc;
		WIPE(desc);
		desc.Width = aSize[0];
		desc.Height = aSize[1];
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = aFormat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;


		ID3D11Texture2D* texture;
		result = device->CreateTexture2D(&desc, nullptr, &texture);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create texture");
			return {};
		}

		ID3D11ShaderResourceView* shaderResource;
		result = device->CreateShaderResourceView(texture, nullptr, &shaderResource);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create shader resource");
			return {};
		}

		Texture returnVal;
		CreateTexture(texture, returnVal);

		returnVal.myShaderResource = shaderResource;
		return returnVal;
	}

	void TextureFactory::CreateTexture(ID3D11Texture2D* aTexture, Texture& aTextureObject)
	{
		GraphicsFramework& framework = GraphicsEngine::GetInstance().GetFrameWork();
		ID3D11Device* device = framework.GetDevice();

		HRESULT result;

		ID3D11RenderTargetView* renderTarget;
		result = device->CreateRenderTargetView(aTexture, nullptr, &renderTarget);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create render target");
			return;
		}
		aTextureObject.myRenderTarget = renderTarget;
		D3D11_VIEWPORT* viewport = nullptr;
		if (aTexture)
		{
			D3D11_TEXTURE2D_DESC desc;
			aTexture->GetDesc(&desc);
			viewport = new D3D11_VIEWPORT({ 0.f, 0.f, static_cast<float>(desc.Width), static_cast<float>(desc.Height), 0.f, 1.f });
		}
		aTextureObject.myTexture = aTexture;
		aTextureObject.myViewport = viewport;
	}

	DepthTexture TextureFactory::CreateDepth(tools::V2ui aSize, const std::string& aName)
	{
		GraphicsFramework& framework = GraphicsEngine::GetInstance().GetFrameWork();
		ID3D11Device* device = framework.GetDevice();

		HRESULT result;

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = aSize[0];
		desc.Height = aSize[1];
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		ID3D11Texture2D* texture;
		result = device->CreateTexture2D(&desc, nullptr, &texture);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create texture");
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		dsv_desc.Flags = 0;
		dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Texture2D.MipSlice = 0;

		ID3D11DepthStencilView* depth;
		result = device->CreateDepthStencilView(texture, &dsv_desc, &depth);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create depth stencil");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
		sr_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		sr_desc.Texture2D.MostDetailedMip = 0;
		sr_desc.Texture2D.MipLevels = static_cast<UINT>(-1);

		ID3D11ShaderResourceView* resourceView;
		result = device->CreateShaderResourceView(texture, &sr_desc, &resourceView);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Could not create depth stencil");
		}

		DepthTexture returnVal;

		returnVal.myTexture = texture;
		returnVal.myDepth = depth;
		returnVal.myShaderResource = resourceView;

		return returnVal;
	}

	graphics::GBuffer TextureFactory::CreateGBuffer(const tools::V2ui& aSize, const std::string& aName)
	{
		GraphicsFramework& framework = graphics::GraphicsEngine::GetInstance().GetFrameWork();
		ID3D11Device* device = framework.GetDevice();

		HRESULT result;

		std::array<DXGI_FORMAT, static_cast<int>(graphics::GBuffer::Channel::Count)> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R16G16B16A16_SNORM,
			DXGI_FORMAT_R16G16B16A16_SNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
		};

		D3D11_TEXTURE2D_DESC desc;
		WIPE(desc);
		desc.Width = aSize[0];
		desc.Height = aSize[1];
		desc.MipLevels = 1;
		desc.ArraySize = 1;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		GBuffer buffer;
		for (size_t i = 0; i < ENUM_CAST(GBuffer::Channel::Count); i++)
		{
			desc.Format = formats[i];
			result = device->CreateTexture2D(&desc, nullptr, &buffer.myTextures[i]);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Could not create Gbuffer");
				return {};
			}

			result = device->CreateRenderTargetView(buffer.myTextures[i], nullptr, &buffer.myRenderTargets[i]);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Could not create Gbuffer render target");
				return {};
			}

			result = device->CreateShaderResourceView(buffer.myTextures[i], nullptr, &buffer.myShaderResources[i]);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Could not create Gbuffer shader resource");
				return {};
			}
		}


		buffer.myViewport = new D3D11_VIEWPORT({ 0.f, 0.f, static_cast<float>(aSize[0]), static_cast<float>(aSize[1]), 0.f, 1.f });

		return buffer;
	}
}