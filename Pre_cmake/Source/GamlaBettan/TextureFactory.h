#pragma once

#include "CommonUtilities/Vector2.hpp"
#include "Texture.h"
#include "GBuffer.h"

class DirectX11Framework;
struct ID3D11Texture2D;

class TextureFactory 
	: public CommonUtilities::Singleton<TextureFactory>
{
public:
	TextureFactory() = default;
	~TextureFactory() = default;

	bool Init(DirectX11Framework* aFramework);
	
	UpdatableTexture* CreateUpdatableTexture(CommonUtilities::Vector2<unsigned int> aSize, const std::string& aName);
	Texture CreateTexture(CommonUtilities::Vector2<unsigned int> aSize, DXGI_FORMAT aFormat, const std::string& aName);
	void CreateTexture(ID3D11Texture2D* aTexture, Texture& aTextureObject);
	Texture CreateDepth(CommonUtilities::Vector2<unsigned int> aSize, const std::string& aName);
	GBuffer CreateGBuffer(const CommonUtilities::Vector2<unsigned int>& aSize, const std::string& aName);

private:
	DirectX11Framework* myFramework = nullptr;
};

