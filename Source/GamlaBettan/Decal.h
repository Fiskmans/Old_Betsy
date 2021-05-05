#pragma once
#include <ShaderCompiler.h>

#if __INTELLISENSE__
#include <pch.h>
#endif // __INTELLISENSE__


struct Decal
{
	Camera* myCamera;
	std::vector<Texture*> myTextures;
	ID3D11ShaderResourceView* myDepth = nullptr;
	PixelShader* myPixelShader = nullptr;
	float myRange;
	float myIntensity;
	float myTimestamp;
	V4F myCustomData;
};