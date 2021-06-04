#include "pch.h"
#include "Sprite.h"
#include <d3d11.h>

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
