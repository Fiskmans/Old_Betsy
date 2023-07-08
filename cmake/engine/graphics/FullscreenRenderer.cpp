
#include "engine/graphics/FullscreenRenderer.h"
#include "engine/graphics/GraphicsFramework.h"
#include "engine/graphics/GraphicEngine.h"

#include "engine/assets/AssetManager.h"

#include <d3d11.h>

namespace engine::graphics
{
	FullscreenRenderer::FullscreenRenderer()
	{
		for (size_t i = 0; i < static_cast<size_t>(Shader::COUNT); i++)
		{
			myPixelShaders[i] = nullptr;
		}
	}

	bool FullscreenRenderer::Init()
	{
		myVertexShader = AssetManager::GetInstance().GetVertexShader("Fullscreen.hlsl");

		std::array<std::string, static_cast<int>(Shader::COUNT)> filePaths;
		filePaths[static_cast<int>(Shader::MERGE)] = "fullscreen/Merge.hlsl";
		filePaths[static_cast<int>(Shader::LUMINANCE)] = "fullscreen/Lumiance.hlsl";
		filePaths[static_cast<int>(Shader::GAUSSIANHORIZONTAL)] = "fullscreen/GaussianHorizontal.hlsl";
		filePaths[static_cast<int>(Shader::GAUSSIANVERTICAL)] = "fullscreen/GaussianVertical.hlsl";
		filePaths[static_cast<int>(Shader::COPY)] = "fullscreen/Copy.hlsl";
		filePaths[static_cast<int>(Shader::DeferedToToon)] = "fullscreen_deferred/ToonShader.hlsl";
		filePaths[static_cast<int>(Shader::DiscardFull)] = "fullscreen/DiscardFull.hlsl";
		filePaths[static_cast<int>(Shader::SSAO)] = "fullscreen_deferred/SSAO.hlsl";
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
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		context->GSSetShader(nullptr, nullptr, 0);

		context->VSSetShader(myVertexShader.Access().myShader, nullptr, 0);
		context->PSSetShader(myPixelShaders[static_cast<int>(aEffect)].Access().myShader, nullptr, 0);

		context->Draw(3, 0);
	}

	void FullscreenRenderer::Render(const AssetHandle<PixelShaderAsset>& aShader)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		context->GSSetShader(nullptr, nullptr, 0);

		context->VSSetShader(myVertexShader.Access().myShader, nullptr, 0);
		context->PSSetShader(aShader.Access().myShader, nullptr, 0);

		context->Draw(3, 0);
	}
}