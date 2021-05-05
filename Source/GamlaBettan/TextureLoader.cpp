#include "pch.h"
#include "TextureLoader.h"
#include "../Game/perlin_noise.h"

#pragma warning(push)
#pragma warning(disable: 26812)
#include "DirectXTK\Inc\DDSTextureLoader.h"

#include <stdexcept>
#include <d3d11.h>
#include <comdef.h>
#include "DirectX11Framework.h"

class not_ready : public std::exception
{
public:
	not_ready(const char* data) : std::exception(data) {};
};

//TODO: Verify/fix all small leaks in this file

ID3D11ShaderResourceView* GlobalErrorTexture = nullptr;
std::unordered_map<std::string, Texture*> GlobalMemoizedTextures;
std::mutex GlobalMemoizedTextureMutex;
#if USEFILEWATHCER
Tools::FileWatcher GlobalTextureWatcher;


void ReloadTexture(const std::string& aFilePath)
{
	std::lock_guard guard(GlobalMemoizedTextureMutex);
	Texture* oldTexture = GlobalMemoizedTextures[aFilePath];
	if (!oldTexture)
	{
		throw std::exception("Reloading texture that was never loaded, something went terribly wrong");
	}
	GlobalMemoizedTextures.erase(aFilePath);

	ID3D11Device* dev;
	oldTexture->operator ID3D11ShaderResourceView* ()->GetDevice(&dev);

	Texture* newTexture = LoadTexture(dev, aFilePath);
	if (newTexture)
	{
		LOGINFO("Reloaded " + aFilePath + " successully");
		oldTexture->myView->Release();					//release old texture
		oldTexture->myView = newTexture->myView;		//Steal new texture
		newTexture->myView = nullptr;					//orphan new texture from new container
		GlobalMemoizedTextures[aFilePath] = oldTexture;	//update memoized state so further calls to load texture still get the same texture
		delete newTexture;								//clean up shell of new texture
	}
	else
	{
		LOGERROR("Could not reload " + aFilePath);
		GlobalMemoizedTextures[aFilePath] = oldTexture;
	}

}
#endif // USEFILEWATHCER


Texture* LoadTexture(ID3D11Device* aDevice, std::string aFilePath, D3D11_RESOURCE_MISC_FLAG aFlag, D3D11_USAGE aUsage)
{
	std::lock_guard guard(GlobalMemoizedTextureMutex);

	std::wstring wide = std::wstring(aFilePath.begin(), aFilePath.end());
	if (GlobalMemoizedTextures.count(aFilePath) == 0)
	{
		ID3D11ShaderResourceView* tex;
		HRESULT result = DirectX::CreateDDSTextureFromFileEx(aDevice, nullptr, wide.c_str(), 0, aUsage, D3D11_BIND_SHADER_RESOURCE, 0, aFlag, false, nullptr, &tex);
		if (FAILED(result))
		{
			std::wstring replacePath = wide.c_str();
			replacePath.append(L".dds");
			HRESULT result2 = DirectX::CreateDDSTextureFromFileEx(aDevice, nullptr, replacePath.c_str(), 0, aUsage, D3D11_BIND_SHADER_RESOURCE, 0, aFlag, false, nullptr, &tex);
			result = result2;
		}

		if (FAILED(result))
		{
			SYSERROR("Could not load Texture", std::string(aFilePath.begin(), aFilePath.end()));
			if (GlobalErrorTexture)
			{
				ID3D11Device* texDevice = nullptr;
				GlobalErrorTexture->GetDevice(&texDevice);
				if (texDevice == aDevice)
				{
					tex = GlobalErrorTexture;
				}
				else
				{
					SYSCRASH("Error texture is not of same device as loading device... just what are you doing?");
					throw std::invalid_argument("Error texture is not of same device as loading device... just what are you doing?");
				}
			}
			else
			{
				SYSCRASH("Texture loader does not have error texture set");
				throw not_ready("Texture loader does not have error texture set");
			}
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
			DirectX11Framework::AddMemoryUsage(desc.Width * desc.Height * DirectX11Framework::FormatToSizeLookup[desc.Format], std::filesystem::path(aFilePath).filename().string(), "Texture2d");
		}

		ID3D11Texture3D* tex3d;
		res->QueryInterface(&tex3d);
		if (tex3d)
		{
			D3D11_TEXTURE3D_DESC desc;
			tex3d->GetDesc(&desc);
			tex3d->Release();
			DirectX11Framework::AddMemoryUsage(desc.Width * desc.Height * desc.Depth * DirectX11Framework::FormatToSizeLookup[desc.Format], std::filesystem::path(aFilePath).filename().string(), "Texture3d");
		}
		res->Release();

		GlobalMemoizedTextures[aFilePath] = new Texture(tex);
#if USEFILEWATHCER
		auto _ = GlobalTextureWatcher.RegisterCallback(aFilePath, std::bind(&ReloadTexture, std::placeholders::_1));
#endif
	}
	GlobalMemoizedTextures[aFilePath]->AddRef();
	return GlobalMemoizedTextures[aFilePath];
}
void Texture::Flush()
{
#if USEFILEWATHCER
	GlobalTextureWatcher.FlushChanges();
#endif
}

Texture* GeneratePerlin(ID3D11Device* aDevice, CommonUtilities::Vector2<size_t> aSize, V2F aScale, unsigned int aSeed, D3D11_RESOURCE_MISC_FLAG aFlag)
{
	typedef CommonUtilities::Vector4<float> vec;

	vec* color = new vec[aSize.x * aSize.y];

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
			color[y * aSize.x + x].x = CAST(float, generators[0].noise(x / CAST(double, aScale.x) / aSize.x * 255.0, y / CAST(double, aScale.y) / aSize.y * 255.0, 0.0));
			color[y * aSize.x + x].y = CAST(float, generators[1].noise(x / CAST(double, aScale.x) / aSize.x * 255.0, y / CAST(double, aScale.y) / aSize.y * 255.0, 1.0));
			color[y * aSize.x + x].z = CAST(float, generators[2].noise(x / CAST(double, aScale.x) / aSize.x * 255.0, y / CAST(double, aScale.y) / aSize.y * 255.0, 2.0));
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

	HRESULT res = aDevice->CreateTexture2D(&desc, &data, &tex);

	if (FAILED(res))
	{
		SYSERROR("Could not generate texture", "perlin");
		return nullptr;
	}
	ID3D11ShaderResourceView* view;
	res = aDevice->CreateShaderResourceView(tex, nullptr, &view);
	tex->Release();
	if (FAILED(res))
	{
		SYSERROR("Could not generate texture", "perlin");
		return nullptr;
	}

	return new Texture(view);
}

void SetErrorTexture(ID3D11Device* aDevice, std::string aFilePath)
{
	GlobalErrorTexture = *LoadTexture(aDevice, aFilePath);
}

bool IsErrorTexture(Texture* aTexture)
{
	return *aTexture == GlobalErrorTexture;
}

void ReleaseTexture(Texture* aTexture)
{
	if (aTexture->Release_internal())
	{
		delete aTexture;
	}
}


#pragma warning(pop)
