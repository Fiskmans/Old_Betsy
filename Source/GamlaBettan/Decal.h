#pragma once

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