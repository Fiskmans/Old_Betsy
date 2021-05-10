#pragma once

#include "Singleton.hpp"
#include "Asset.h"
#include "ShaderFlags.h"

#include <unordered_map>

class AssetManager
	: public CommonUtilities::Singleton<AssetManager>
{
public:

	AssetHandle GetTexture(const std::string& aPath);
	AssetHandle GetModel(const std::string& aPath);
	AssetHandle GetSkybox(const std::string& aPath);
	AssetHandle GetPixelShader(const std::string& aPath, ShaderFlags aFlags);

private:

	ModelLoader* myModelLoader;

	std::unordered_map<std::string, Asset*> myCachedModels;
	std::unordered_map<std::string, Asset*> myCachedSkyboxes;
};

