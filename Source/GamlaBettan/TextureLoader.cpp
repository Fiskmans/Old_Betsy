#include "pch.h"
#include "TextureLoader.h"
#include "Tools\perlin_noise.h"

#include "DirectXTK\Inc\DDSTextureLoader.h"

#include <stdexcept>
#include <d3d11.h>
#include <comdef.h>
#include "DirectX11Framework.h"

TextureLoader::TextureLoader(ID3D11Device* aDevice)
{
	myDevice = aDevice;
}

Asset* TextureLoader::LoadTexture(const std::string& aFile, bool aFailSilenty)
{
	return LoadTextureInternal(aFile, D3D11_RESOURCE_MISC_FLAG(), aFailSilenty);
}

Asset* TextureLoader::LoadCubeTexture(const std::string& aFile)
{
	return LoadTextureInternal(aFile, D3D11_RESOURCE_MISC_TEXTURECUBE, false);
}

Asset* TextureLoader::GeneratePerlin(V2ui aSize, V2f aScale, unsigned int aSeed)
{
	V4F* color = new V4F[aSize.x * aSize.y];

	PerlinNoise generators[3]
	{
		PerlinNoise(aSeed),
		PerlinNoise(aSeed * 2),
		PerlinNoise(aSeed * 3),
	};

	for (size_t y = 0; y < aSize.y; y++)
	{
		for (size_t x = 0; x < aSize.x; x++)
		{
			color[y * aSize.x + x].x = generators[0].noise(x / aScale.x / aSize.x * 255.0f, y / aScale.y / aSize.y * 255.0f, 0.0f);
			color[y * aSize.x + x].y = generators[1].noise(x / aScale.x / aSize.x * 255.0f, y / aScale.y / aSize.y * 255.0f, 1.0f);
			color[y * aSize.x + x].z = generators[2].noise(x / aScale.x / aSize.x * 255.0f, y / aScale.y / aSize.y * 255.0f, 2.0f);
			color[y * aSize.x + x].w = 0;
		}
	}
	D3D11_TEXTURE2D_DESC desc;
	WIPE(desc);
	desc.MipLevels = 1;
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.ArraySize = 1;
	desc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	ID3D11Texture2D* tex;

	D3D11_SUBRESOURCE_DATA data;
	WIPE(data);
	data.pSysMem = color;
	data.SysMemPitch = CAST(UINT, sizeof(CommonUtilities::Vector4<float>) * aSize.x);
	data.SysMemSlicePitch = CAST(UINT, sizeof(CommonUtilities::Vector4<float>) * aSize.x * aSize.y);

	HRESULT res = myDevice->CreateTexture2D(&desc, &data, &tex);

	if (FAILED(res))
	{
		SYSERROR("Could not generate texture", "perlin");
		return nullptr;
	}
	ID3D11ShaderResourceView* view;
	res = myDevice->CreateShaderResourceView(tex, nullptr, &view);
	tex->Release();
	if (FAILED(res))
	{
		SYSERROR("Could not generate texture", "perlin");
		return nullptr;
	}

	return new TextureAsset(view);
}

Asset* TextureLoader::LoadTextureInternal(const std::string& aFile, D3D11_RESOURCE_MISC_FLAG aFlags, bool aFailSilenty)
{
	std::wstring wide = std::wstring(aFile.begin(), aFile.end());

	ID3D11ShaderResourceView* tex;
	HRESULT result = DirectX::CreateDDSTextureFromFileEx(myDevice, nullptr, wide.c_str(), 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, 0, aFlags, false, nullptr, &tex);

	if (FAILED(result))
	{
		if (!aFailSilenty)
		{
			SYSERROR("Failed to load texture", aFile);
		}
		return nullptr;
	}

	ID3D11Resource* res;
	tex->GetResource(&res);
	ID3D11Texture2D* tex2d;
	res->QueryInterface(&tex2d);
	if (tex2d)
	{
		D3D11_TEXTURE2D_DESC desc;
		tex2d->GetDesc(&desc);
		tex2d->Release();
		DirectX11Framework::AddGraphicsMemoryUsage(desc.Width * desc.Height * DirectX11Framework::FormatToSizeLookup[desc.Format], aFile, "Texture2d");
	}

	ID3D11Texture3D* tex3d;
	res->QueryInterface(&tex3d);
	if (tex3d)
	{
		D3D11_TEXTURE3D_DESC desc;
		tex3d->GetDesc(&desc);
		tex3d->Release();
		DirectX11Framework::AddGraphicsMemoryUsage(desc.Width * desc.Height * desc.Depth * DirectX11Framework::FormatToSizeLookup[desc.Format], aFile, "Texture3d");
	}
	res->Release();

	return new TextureAsset(tex);
}
