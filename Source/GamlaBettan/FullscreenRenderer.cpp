#include "pch.h"
#include "FullscreenRenderer.h"
#include "DirectX11Framework.h"
#include <d3d11.h>
#include "ShaderCompiler.h"
#include "AssetManager.h"

FullscreenRenderer::FullscreenRenderer()
{
	for (size_t i = 0; i < CAST(size_t, Shader::COUNT); i++)
	{
		myPixelShaders[i] = nullptr;
	}
}

bool FullscreenRenderer::Init(DirectX11Framework* aFramework)
{
	if (!aFramework)
	{
		SYSCRASH("FullscreenRenderer got no framework to work on!!.!1!");
		return false;
	}
	myContext = aFramework->GetContext();
	if (!myContext)
	{
		SYSCRASH("Got Invalid framework in fullscreen renderer");
		return false;
	}

	ID3D11Device* device = aFramework->GetDevice();

	myVertexShader = AssetManager::GetInstance().GetVertexShader("Fullscreen.hlsl");

	std::array<std::string, static_cast<int>(Shader::COUNT)> filePaths;
	filePaths[static_cast<int>(Shader::MERGE)] = "fullscreen/Merge.hlsl";
	filePaths[static_cast<int>(Shader::LUMINANCE)] = "fullscreen/Lumiance.hlsl";
	filePaths[static_cast<int>(Shader::GAUSSIANHORIZONTAL)] = "fullscreen/GaussianHorizontal.hlsl";
	filePaths[static_cast<int>(Shader::GAUSSIANVERTICAL)] = "fullscreen/GaussianVertical.hlsl";
	filePaths[static_cast<int>(Shader::COPY)] = "fullscreen/Copy.hlsl";
	filePaths[static_cast<int>(Shader::PBREnvironmentLight)] = "fullscreen_deferred/ToonShader.hlsl";
	filePaths[static_cast<int>(Shader::PBRPointLight)] = "fullscreen_deferred/PBRPoint.hlsl";
	filePaths[static_cast<int>(Shader::DiscardFull)] = "fullscreen/DiscardFull.hlsl";
	filePaths[static_cast<int>(Shader::SSAO)] = "fullscreen/SSAO.hlsl";
	filePaths[static_cast<int>(Shader::PBRSpotLight)] = "fullscreen_deferred/PBRspot.hlsl";
	filePaths[static_cast<int>(Shader::Cloud)] = "fullscreen/Clouds.hlsl";
	filePaths[static_cast<int>(Shader::EdgeDetection)] = "fullscreen/EdgeDetection.hlsl";
	filePaths[static_cast<int>(Shader::ConditionalGAUSSIANHORIZONTAL)] = "fullscreen/ConditionalGaussianHorizontal.hlsl";
	filePaths[static_cast<int>(Shader::ConditionalGAUSSIANVERTICAL)] = "fullscreen/ConditionalGaussianVertical.hlsl";

	for (size_t i = 0; i < filePaths.size(); i++)
	{
		myPixelShaders[i] = AssetManager::GetInstance().GetPixelShader(filePaths[i]);
	}



	return true;
}

void FullscreenRenderer::Render(Shader aEffect)
{
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	myContext->GSSetShader(nullptr, nullptr, 0);

	myContext->VSSetShader(myVertexShader.GetAsVertexShader(), nullptr, 0);
	myContext->PSSetShader(myPixelShaders[static_cast<int>(aEffect)].GetAsPixelShader(), nullptr, 0);

	myContext->Draw(3,0);
}

void FullscreenRenderer::Render(const AssetHandle& aShader)
{
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	myContext->GSSetShader(nullptr, nullptr, 0);

	myContext->VSSetShader(myVertexShader.GetAsVertexShader(), nullptr, 0);
	myContext->PSSetShader(aShader.GetAsPixelShader(), nullptr, 0);

	myContext->Draw(3, 0);
}
