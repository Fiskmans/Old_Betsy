#include "pch.h"
#include "SpriteFactory.h"
#include "Sprite.h"
#include "SpriteInstance.h"
#include <fstream>
#include <DDSTextureLoader.h>
#include "ShaderCompiler.h"
#include "DirectX11Framework.h"
#include "TextureLoader.h"

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
	SpriteInstance* newSpriteInstance = new SpriteInstance(GetSprite(aDDSPath, false));
	return newSpriteInstance;
}

SpriteInstance* SpriteFactory::CreateVideoSprite()
{
	SpriteInstance* newSpriteInstance = new SpriteInstance(GetSprite(myVideoSpritePath, true));
	return newSpriteInstance;
}

Sprite* SpriteFactory::GetSprite(const std::string& aDDSPath, const bool aIsMovie)
{
	if (mySprites.find(aDDSPath) == mySprites.end())
	{
		Sprite* newSprite = LoadSprite(aDDSPath, aIsMovie);
		if (!newSprite)
		{
			SYSERROR("Failed to load sprite in SpriteFactory.GetSprite()", aDDSPath);
			return nullptr;
		}
		return newSprite;
	}
	else
	{
		return mySprites[aDDSPath];
	}

	SYSERROR("Oh no wat?", "");
	return nullptr;
}

Sprite* SpriteFactory::LoadSprite(const std::string& aDDSPath, const bool aIsMovie)
{
	HRESULT result;

	std::wstring texturePath(aDDSPath.begin(), aDDSPath.end());
	Texture* texture = LoadTexture(myDevice,aDDSPath);
	UINT ddsWidth = 0;
	UINT ddsHeight = 0;
	if (aIsMovie)
	{
		ddsWidth = Sprite::ourWindowSize.x;
		ddsHeight = Sprite::ourWindowSize.y;
	}
	else
	{
		ID3D11Resource* resource;
		texture->operator ID3D11ShaderResourceView* ()->GetResource(&resource);
		D3D11_TEXTURE2D_DESC desc;
		((ID3D11Texture2D*)resource)->GetDesc(&desc);
		ddsWidth = desc.Width;
		ddsHeight = desc.Height;
	}


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

