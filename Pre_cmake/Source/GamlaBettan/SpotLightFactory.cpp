#include "pch.h"
#include "SpotLightFactory.h"
#include "CameraFactory.h"
#include "AssetManager.h"

void SpotLightFactory::Init(ID3D11Device* aDevice)
{
	myDevice = aDevice;
}

SpotLight* SpotLightFactory::LoadSpotlight(const std::string& aTexturePath, float aFOV, float aRange, float aIntensity, V3F aRotation, V3F aPosition)
{
	SpotLight* light = new SpotLight();
	light->myCamera = CCameraFactory::CreateCamera(aFOV,false,20.f,aRange);
	light->myCamera->Rotate(aRotation);
	light->myCamera->SetPosition(aPosition);
	light->myCamera->SetResolution(V2ui(128, 128));
	light->myRange = aRange;
	light->myIntensity = aIntensity;
	light->myTexture = AssetManager::GetInstance().GetTexture(aTexturePath);
	return light;
}

SpotLight* SpotLightFactory::LoadSpotlight(const std::string& aTexturePath, float aFOV, float aRange, float aIntensity, Camera* aCameraToCopyFrom)
{
	SpotLight* light = new SpotLight();
	light->myCamera = CCameraFactory::CreateCamera(aFOV, false, 20.f, aRange);
	light->myCamera->SetTransform(aCameraToCopyFrom->GetTransform());
	light->myCamera->SetResolution(V2ui(128, 128));
	light->myRange = aRange;
	light->myIntensity = aIntensity;
	light->myTexture = AssetManager::GetInstance().GetTexture(aTexturePath);
	return light;
}