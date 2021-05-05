#pragma once
#include "Vector4.hpp"
#include <array>
#include "Macros.h"

class GBuffer
{
public:
	enum class Textures
	{
		Postion,
		Albedo,
		Normal,
		VertexNormal,
		Metalness,
		Roughness,
		AmbientOcclusion,
		Emissive,
		Count
	};

	GBuffer();
	~GBuffer();

	void ClearTextures(const V4F aClearColor = V4F(0.f, 0.f, 0.f, 0.f));
	void SetAsActiveTarget(class FullscreenTexture* aDepth = nullptr);
	void SetAsActiveTarget(const Textures aResource, class FullscreenTexture* aDepth = nullptr);
	void SetAsResourceOnSlot(const Textures aResource, const unsigned int aSlot);
	void SetAllAsResources();
	void CopyTo(GBuffer* aOther, ID3D11DeviceContext* aContext);

	void Release();

private:
	friend class FullscreenTextureFactory;

	std::array<struct ID3D11Texture2D*, ENUM_CAST(Textures::Count)> myTextures;
	std::array<struct ID3D11RenderTargetView*, ENUM_CAST(Textures::Count)> myRenderTargets;
	std::array<struct ID3D11ShaderResourceView*, ENUM_CAST(Textures::Count)> myShaderResources;
	struct D3D11_VIEWPORT* myViewport;

	struct ID3D11DeviceContext* myContext;
};

