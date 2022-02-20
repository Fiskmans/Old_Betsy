#pragma once
#include <array>
class PixelShader;

class DirectX11Framework;
class FullscreenRenderer
{
public:
	enum class Shader
	{
		COPY,
		LUMINANCE,
		GAUSSIANHORIZONTAL,
		GAUSSIANVERTICAL,
		MERGE,
		PBREnvironmentLight,
		PBRPointLight,
		DiscardFull,
		SSAO,
		PBRSpotLight,
		Cloud,
		EdgeDetection,
		ConditionalGAUSSIANHORIZONTAL,
		ConditionalGAUSSIANVERTICAL,
		COUNT
	};

public:
	FullscreenRenderer();
	~FullscreenRenderer() = default;

	bool Init(DirectX11Framework* aFramework);

	void Render(Shader aEffect);
	void Render(const AssetHandle& aShader);

private:
	struct ID3D11DeviceContext* myContext = nullptr;
	AssetHandle myVertexShader = nullptr;
	std::array<AssetHandle, static_cast<int>(Shader::COUNT)> myPixelShaders;

};