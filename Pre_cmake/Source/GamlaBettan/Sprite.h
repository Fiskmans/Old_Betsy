#pragma once
#include <d3dcommon.h>
#include "Observer.hpp"

struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11ShaderResourceView;

class Sprite
{
public:
	struct SpriteData
	{
		AssetHandle myTexture;
		V2ui mySize = { 0, 0 };
		std::string myFilePath;
	};

	void Init(const SpriteData& aSpriteData);
	SpriteData& GetSpriteData();
	const SpriteData& GetSpriteData() const;

private:
	SpriteData mySpriteData;
	
};