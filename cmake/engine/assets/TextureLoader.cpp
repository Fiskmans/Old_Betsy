#include "engine/assets/TextureLoader.h"

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/GraphicsFramework.h"

#include "tools/Logger.h"

#include <string>

#include <d3d11.h>
#include <DDSTextureLoader.h>


namespace engine::assets
{
	Asset* TextureLoader::LoadTexture(const std::string& aFile, bool aFailSilenty)
	{
		return LoadTextureInternal(aFile, 0, aFailSilenty);
	}

	Asset* TextureLoader::LoadCubeTexture(const std::string& aFile)
	{
		return LoadTextureInternal(aFile, D3D11_RESOURCE_MISC_TEXTURECUBE, false);
	}

	Asset* TextureLoader::MakeTexture(tools::V2ui aResolution, DXGI_FORMAT aFormat)
	{
		return nullptr;
	}

	//Asset* TextureLoader::GeneratePerlin(V2ui aSize, V2f aScale, unsigned int aSeed)
	//{
	//	V4F* color = new V4F[aSize.x * aSize.y];
	//
	//	PerlinNoise generators[3]
	//	{
	//		PerlinNoise(aSeed),
	//		PerlinNoise(aSeed * 2),
	//		PerlinNoise(aSeed * 3),
	//	};
	//
	//	for (size_t y = 0; y < aSize.y; y++)
	//	{
	//		for (size_t x = 0; x < aSize.x; x++)
	//		{
	//			color[y * aSize.x + x].x = static_cast<float>(generators[0].noise(x / aScale.x / aSize.x * 255.0f, y / aScale.y / aSize.y * 255.0f, 0.0f));
	//			color[y * aSize.x + x].y = static_cast<float>(generators[1].noise(x / aScale.x / aSize.x * 255.0f, y / aScale.y / aSize.y * 255.0f, 1.0f));
	//			color[y * aSize.x + x].z = static_cast<float>(generators[2].noise(x / aScale.x / aSize.x * 255.0f, y / aScale.y / aSize.y * 255.0f, 2.0f));
	//			color[y * aSize.x + x].w = 0.f;
	//		}
	//	}
	//	D3D11_TEXTURE2D_DESC desc;
	//	WIPE(desc);
	//	desc.MipLevels = 1;
	//	desc.Width = aSize.x;
	//	desc.Height = aSize.y;
	//	desc.ArraySize = 1;
	//	desc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	//	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//	desc.SampleDesc.Count = 1;
	//	desc.SampleDesc.Quality = 0;
	//
	//	ID3D11Texture2D* tex;
	//
	//	D3D11_SUBRESOURCE_DATA data;
	//	WIPE(data);
	//	data.pSysMem = color;
	//	data.SysMemPitch = CAST(UINT, sizeof(CommonUtilities::Vector4<float>) * aSize.x);
	//	data.SysMemSlicePitch = CAST(UINT, sizeof(CommonUtilities::Vector4<float>) * aSize.x * aSize.y);
	//
	//	HRESULT res = myDevice->CreateTexture2D(&desc, &data, &tex);
	//
	//	if (FAILED(res))
	//	{
	//		SYSERROR("Could not generate texture", "perlin");
	//		return nullptr;
	//	}
	//	ID3D11ShaderResourceView* view;
	//	res = myDevice->CreateShaderResourceView(tex, nullptr, &view);
	//	tex->Release();
	//	if (FAILED(res))
	//	{
	//		SYSERROR("Could not generate texture", "perlin");
	//		return nullptr;
	//	}
	//
	//	return new TextureAsset(view);
	//}

	Asset* TextureLoader::LoadTextureInternal(const std::string& aFile, size_t aFlags, bool aFailSilenty)
	{
		std::wstring wide = std::wstring(aFile.begin(), aFile.end());

		ID3D11ShaderResourceView* tex;
		HRESULT result = DirectX::CreateDDSTextureFromFileEx(
			graphics::GraphicsEngine::GetInstance().GetFrameWork().GetDevice(),
			graphics::GraphicsEngine::GetInstance().GetFrameWork().GetContext(),
			wide.c_str(), 
			0, 
			D3D11_USAGE_IMMUTABLE, 
			D3D11_BIND_SHADER_RESOURCE, 
			0,
			aFlags,
			DirectX::DDS_LOADER_FORCE_SRGB,
			nullptr,
			&tex,
			nullptr);

		if (FAILED(result))
		{
			if (!aFailSilenty)
			{
				LOG_ERROR("Failed to load texture", aFile);
			}
			return nullptr;
		}

		return new TextureAsset(tex);
	}

}