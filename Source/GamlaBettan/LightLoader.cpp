#include "pch.h"
#include "LightLoader.h"
#include "Environmentlight.h"
#include <DDSTextureLoader.h>
#include "CameraFactory.h"
#include "PointLight.h"

bool LightLoader::Init(ID3D11Device* aDevice)
{
	myDevice = aDevice;
	return true;
}

EnvironmentLight* LightLoader::LoadLight(const std::string& aFilePath)
{
	HRESULT result;

	std::wstring fileName = std::wstring(aFilePath.begin(), aFilePath.end());
	ID3D11ShaderResourceView* shaderResourceView;
	result = DirectX::CreateDDSTextureFromFileEx(myDevice,
		fileName.c_str(),
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,
		false,
		nullptr,
		&shaderResourceView,
		nullptr);
	if (FAILED(result))
	{
		SYSERROR("Could not load environment light",aFilePath);
		return nullptr;
	}

	EnvironmentLight* light = new EnvironmentLight;
	light->myTexture = shaderResourceView;
	light->myIntensity = 1.f;
	//Todo: Add dir and color
	return light;
}
