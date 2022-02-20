#include "pch.h"
#include "LightLoader.h"
#include "Environmentlight.h"
#include "DirectXTK\Inc\DDSTextureLoader.h"
#include "CameraFactory.h"
#include "PointLight.h"
#include "AssetManager.h"

bool LightLoader::Init(ID3D11Device* aDevice)
{
	myDevice = aDevice;
	return true;
}

EnvironmentLight* LightLoader::LoadLight(const std::string& aFilePath)
{
	EnvironmentLight* light = new EnvironmentLight;
	light->myTexture = AssetManager::GetInstance().GetCubeTexture(aFilePath);
	light->myIntensity = 1.f;
	//Todo: Add dir and color
	return light;
}
