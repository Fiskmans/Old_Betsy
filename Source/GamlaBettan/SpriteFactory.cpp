#include "pch.h"
#include "SpriteFactory.h"
#include "Sprite.h"
#include "SpriteInstance.h"
#include <fstream>
#include "DirectXTK\Inc\DDSTextureLoader.h"
#include "ShaderCompiler.h"
#include "DirectX11Framework.h"
#include "TextureLoader.h"
#include "AssetManager.h"

SpriteFactory::SpriteFactory()
{
	myDevice = nullptr;
}

SpriteFactory::~SpriteFactory()
{

	for (auto& i : mySprites)
	{
		SAFE_DELETE(i.second);
	}
	mySprites["!"] = nullptr;
}

bool SpriteFactory::Init(DirectX11Framework* aFramework)
{

	if (aFramework->GetDevice() == nullptr)
	{
		return false;
	}

	myDevice = aFramework->GetDevice();
	return true;
}

SpriteInstance* SpriteFactory::CreateSprite(const std::string& aDDSPath)
{
	SpriteInstance* newSpriteInstance = new SpriteInstance(GetSprite(aDDSPath));
	return newSpriteInstance;
}


Sprite* SpriteFactory::GetSprite(const std::string& aDDSPath)
{
	if (mySprites.find(aDDSPath) == mySprites.end())
	{
		Sprite* newSprite = LoadSprite(aDDSPath);
		if (!newSprite)
		{
			SYSERROR("Failed to load sprite in SpriteFactory.GetSprite()", aDDSPath);
			return nullptr;
		}
		mySprites[aDDSPath] = newSprite;
		return newSprite;
	}

	return mySprites[aDDSPath];
}

Sprite* SpriteFactory::LoadSprite(const std::string& aDDSPath)
{
	HRESULT result;

	std::wstring texturePath(aDDSPath.begin(), aDDSPath.end());
	AssetHandle texture = AssetManager::GetInstance().GetTexture(aDDSPath);
	UINT ddsWidth = 0;
	UINT ddsHeight = 0;
	ID3D11Resource* resource;
	texture.GetAsTexture()->GetResource(&resource);

	ID3D11Texture2D* tex2d;
	resource->QueryInterface(&tex2d);
	if (tex2d)
	{
		D3D11_TEXTURE2D_DESC desc;
		tex2d->GetDesc(&desc);
		ddsWidth = desc.Width;
		ddsHeight = desc.Height;
		tex2d->Release();
	}

	resource->Release();


	Sprite* newSprite = new Sprite();

	Sprite::SpriteData spriteData;
	spriteData.myTexture = texture;
	spriteData.mySize.y = ddsHeight;
	spriteData.mySize.x = ddsWidth;
	spriteData.myFilePath = aDDSPath;

	newSprite->Init(spriteData);
	mySprites[aDDSPath] = newSprite;
	return mySprites[aDDSPath];
}

