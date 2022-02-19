#include "pch.h"
#include "DecalFactory.h"
#include "CameraFactory.h"
#include "TextureLoader.h"
#include <TimeHelper.h>

#include "AssetManager.h"

void DecalFactory::Init(ID3D11Device* aDevice)
{
	myDevice = aDevice;
}


Decal* DecalFactory::LoadDecal(const std::string& aDecalPath, V3F aRotation, V3F aPosition)
{
	FiskJSON::Object root;
	try
	{
		root.Parse(Tools::ReadWholeFile(aDecalPath));
	}
	catch (FiskJSON::Invalid_JSON& aError)
	{
		SYSERROR("Json is invalid in file " + aDecalPath, aError.what());
		return nullptr;
	}
	std::string shaderFilePath;
	float fov;
	float range;
	if (root["ShaderPath"].GetIf(shaderFilePath) &&
		root["FOV"].GetIf(fov) &&
		root["Range"].GetIf(range))
	{
		Decal* decal = new Decal();

		std::string textureFilePath;
		if (root["Texture"].GetIf(textureFilePath))
		{
			decal->myTextures.push_back(AssetManager::GetInstance().GetTexture(textureFilePath));
		};
		for (auto& i : root["Texture"].Get<FiskJSON::ArrayWrapper>())
		{
			if (i.GetIf(textureFilePath))
			{
				decal->myTextures.push_back(AssetManager::GetInstance().GetTexture(textureFilePath));
			};
		}

		decal->myCamera = CCameraFactory::CreateCamera(fov, false, 20.f, range);
		decal->myCamera->Rotate(TORAD(aRotation));
		decal->myCamera->SetPosition(aPosition);
		decal->myCamera->SetResolution(V2ui(128, 128));
		decal->myRange = range;
		decal->myTimestamp = Tools::GetTotalTime();
		decal->myPixelShader = AssetManager::GetInstance().GetPixelShader(shaderFilePath);
		return decal;
	}
	SYSERROR("Failed to load decal", aDecalPath);
	return nullptr;
}

Decal* DecalFactory::LoadDecal(const std::string& aDecalPath, Camera* aCameraToCopyFrom)
{
	FiskJSON::Object root;
	root.Parse(Tools::ReadWholeFile(aDecalPath));
	std::string shaderFilePath;
	float fov;
	float range;
	if (root["ShaderPath"].GetIf(shaderFilePath) &&
		root["FOV"].GetIf(fov) &&
		root["Range"].GetIf(range))
	{

		Decal* decal = new Decal();
		decal->myCamera = CCameraFactory::CreateCamera(fov, false, 20.f, range);
		decal->myCamera->SetTransform(aCameraToCopyFrom->GetTransform());
		decal->myCamera->SetResolution(V2ui(128, 128));
		decal->myRange = range;
		std::string textureFilePath;
		if (root["Texture"].GetIf(textureFilePath))
		{
			decal->myTextures.push_back(AssetManager::GetInstance().GetTexture(textureFilePath));
		};
		for (auto& i : root["Texture"].Get<FiskJSON::ArrayWrapper>())
		{
			if (i.GetIf(textureFilePath))
			{
				decal->myTextures.push_back(AssetManager::GetInstance().GetTexture(textureFilePath));
			};
		}
		decal->myTimestamp = Tools::GetTotalTime();
		decal->myPixelShader = AssetManager::GetInstance().GetPixelShader(shaderFilePath);

		size_t count = 0;
		for (auto& i : root["CustomData"].Get<FiskJSON::ArrayWrapper>())
		{
			if (count > 3)
			{
				SYSERROR("custom data has too many elements", aDecalPath);
				break;
			}
			if (!i.GetIf(decal->myCustomData[count++]))
			{
				SYSERROR("Custom data can not be converted to float", aDecalPath);
			}
		}

		return decal;
	}
	SYSERROR("Failed to load decal", aDecalPath);
	return nullptr;
}