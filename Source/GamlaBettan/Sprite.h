#pragma once
#include <d3dcommon.h>
#include "Vector2.hpp"
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
		Texture* myTexture = nullptr;
		CommonUtilities::Vector2<unsigned int> mySize = { 0, 0 };
		std::string myFilePath;
	};

	Sprite();
	~Sprite();

	void Init(const SpriteData& aSpriteData);
	SpriteData& GetSpriteData();
	const SpriteData& GetSpriteData() const;

	static V2F ourWindowSize;
private:
	SpriteData mySpriteData;
	
};