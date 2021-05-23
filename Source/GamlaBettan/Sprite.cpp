#include "pch.h"
#include "Sprite.h"
#include <d3d11.h>

V2F Sprite::ourWindowSize = { 1920.f,1080.f };

void Sprite::Init(const SpriteData& aSpriteData)
{
	mySpriteData = aSpriteData;
}

Sprite::SpriteData& Sprite::GetSpriteData()
{
	return mySpriteData;
}

const Sprite::SpriteData& Sprite::GetSpriteData() const
{
	return mySpriteData;
}
