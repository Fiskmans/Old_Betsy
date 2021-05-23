#pragma once
#include <ShaderCompiler.h>

#if __INTELLISENSE__
#include <pch.h>
#endif // __INTELLISENSE__


struct Decal
{
	Camera* myCamera;
	std::vector<AssetHandle> myTextures;
	ID3D11ShaderResourceView* myDepth = nullptr;
	AssetHandle myPixelShader;
	float myRange;
	float myIntensity;
	float myTimestamp;
	V4F myCustomData;
};