#include "pch.h"
#include "FullscreenTextureFactory.h"
#include <d3d11.h>
#include "DirectX11Framework.h"

bool FullscreenTextureFactory::Init(DirectX11Framework* aFramework)
{
	myFramework = aFramework;

	if (!myFramework)
	{
		SYSERROR("Fullscreen texture factory got no framework to work on");
	}
	return !!myFramework;
}

FullscreenTexture FullscreenTextureFactory::CreateTexture(CU::Vector2<unsigned int> aSize, DXGI_FORMAT aFormat, const std::string& aName)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC desc;
	WIPE(desc);
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = aFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	DirectX11Framework::AddMemoryUsage(aSize.x*aSize.y*DirectX11Framework::FormatToSizeLookup[aFormat],aName, "Engine Texture");

	ID3D11Texture2D* texture;
	result = myFramework->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(result))
	{
		SYSERROR("Could not create texture in fullscreen texture factory :c");
		return {};
	}

	FullscreenTexture returnVal;
	returnVal = CreateTexture(texture);

	ID3D11ShaderResourceView* shaderResource;
	result = myFramework->GetDevice()->CreateShaderResourceView(texture, nullptr, &shaderResource);
	if (FAILED(result))
	{
		SYSERROR("Could not create shader resource in fullscreen texture factory :c");
		return {};
	}

	returnVal.myShaderResource = shaderResource;
	return returnVal;
}

FullscreenTexture FullscreenTextureFactory::CreateTexture(ID3D11Texture2D* aTexture)
{
	HRESULT result;

	ID3D11RenderTargetView* renderTarget;
	result = myFramework->GetDevice()->CreateRenderTargetView(aTexture, nullptr, &renderTarget);
	if (FAILED(result))
	{
		SYSERROR("Could not create render target in fullscreen texture factory :c");
		return {};
	}

	D3D11_VIEWPORT* viewport = nullptr;
	if (aTexture)
	{
		D3D11_TEXTURE2D_DESC desc;
		aTexture->GetDesc(&desc);
		viewport = new D3D11_VIEWPORT({ 0.f, 0.f, static_cast<float>(desc.Width), static_cast<float>(desc.Height), 0.f, 1.f });
	}

	FullscreenTexture returnVal;
	returnVal.myContext = myFramework->GetContext();
	returnVal.myTexture = aTexture;
	returnVal.myRenderTarget = renderTarget;
	returnVal.myViewport = viewport;

	return returnVal;
}

FullscreenTexture FullscreenTextureFactory::CreateDepth(CU::Vector2<unsigned int> aSize, const std::string& aName)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	DirectX11Framework::AddMemoryUsage(aSize.x * aSize.y * DirectX11Framework::FormatToSizeLookup[desc.Format], aName, "Engine Depth");
	ID3D11Texture2D* texture;
	result = myFramework->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(result))
	{
		SYSERROR("Could not create texture in fullscreen texture factory :c");
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Flags = 0;
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;

	ID3D11DepthStencilView* depth;
	result = myFramework->GetDevice()->CreateDepthStencilView(texture, &dsv_desc, &depth);
	if (FAILED(result))
	{
		SYSERROR("Could not create depth stencil in fullscreen texture factory :c");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
	sr_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sr_desc.Texture2D.MostDetailedMip = 0;
	sr_desc.Texture2D.MipLevels = -1;

	ID3D11ShaderResourceView* resourceView;
	result = myFramework->GetDevice()->CreateShaderResourceView(texture, &sr_desc, &resourceView);
	if (FAILED(result))
	{
		SYSERROR("Could not create depth stencil in fullscreen texture factory :c");
	}

	D3D11_VIEWPORT* viewport = new D3D11_VIEWPORT({ 0.f, 0.f, static_cast<float>(aSize.x), static_cast<float>(aSize.y), 0.f, 1.f });

	FullscreenTexture returnVal;
	returnVal.myContext = myFramework->GetContext();
	returnVal.myTexture = texture;
	returnVal.myDepth = depth;
	returnVal.myViewport = viewport;
	returnVal.myShaderResource = resourceView;

	return returnVal;
}

GBuffer FullscreenTextureFactory::CreateGBuffer(const CU::Vector2<unsigned int>& aSize, const std::string& aName)
{
	HRESULT result;

	std::array<DXGI_FORMAT, ENUM_CAST(GBuffer::Textures::Count)> formats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM
	};


	ID3D11Device* device = myFramework->GetDevice();

	D3D11_TEXTURE2D_DESC desc;
	WIPE(desc);
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	{
		size_t bpp = 0;
		for (auto& i : formats)
		{
			bpp += DirectX11Framework::FormatToSizeLookup[i];
		}

		DirectX11Framework::AddMemoryUsage(aSize.x * aSize.y * bpp, aName, "Engine GBuffer");
	}

	GBuffer buffer;
	for (size_t i = 0; i < ENUM_CAST(GBuffer::Textures::Count); i++)
	{
		desc.Format = formats[i];
		result = device->CreateTexture2D(&desc,nullptr,&buffer.myTextures[i]);
		if (FAILED(result))
		{
			SYSERROR("Could not Gbuffer.");
			return {};
		}

		result = myFramework->GetDevice()->CreateRenderTargetView(buffer.myTextures[i], nullptr, &buffer.myRenderTargets[i]);
		if (FAILED(result))
		{
			SYSERROR("Could not create Gbuffer render target in fullscreen texture factory :c");
			return {};
		}

		result = myFramework->GetDevice()->CreateShaderResourceView(buffer.myTextures[i], nullptr, &buffer.myShaderResources[i]);
		if (FAILED(result))
		{
			SYSERROR("Could not create Gbuffer shader resource in fullscreen texture factory :c");
			return {};
		}
	}


	buffer.myContext = myFramework->GetContext();
	buffer.myViewport = new D3D11_VIEWPORT({ 0.f, 0.f, static_cast<float>(aSize.x), static_cast<float>(aSize.y), 0.f, 1.f }); 

	return buffer;
}
