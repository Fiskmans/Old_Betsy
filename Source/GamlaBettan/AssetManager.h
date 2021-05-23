#pragma once
#include <unordered_map>

#include "Singleton.hpp"

#include "Asset.h"
#include "ShaderFlags.h"
#include "TextureLoader.h"
#include "ShaderCompiler.h"


class AssetManager
	: public CommonUtilities::Singleton<AssetManager>
{
public:

	void Init(ID3D11Device* aDevice, const std::string& aBaseFolder, const std::string& aBakeFolder);

	void FlushChanges();

	AssetHandle GetTexture(const std::string& aPath);
	AssetHandle GetTextureRelative(const std::string& aBase, const std::string& aPath, bool aFailSilenty = false);
	
	AssetHandle GetCubeTexture(const std::string& aPath);
	AssetHandle GetModel(const std::string& aPath);
	AssetHandle GetSkybox(const std::string& aPath);

	AssetHandle GetPixelShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());
	AssetHandle GetVertexShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());
	AssetHandle GetGeometryShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());

	AssetHandle GetPerlinTexture(CommonUtilities::Vector2<size_t> aSize, V2F aScale, unsigned int aSeed);

	AssetHandle GetJSON(const std::string& aPath);
	AssetHandle GetJSONRelative(const std::string& aBase, const std::string& aPath);

	AssetHandle GetFont(const std::string& aPath);

	AssetHandle GetLevel(const std::string& aPath);

	AssetHandle GetNavMesh(const std::string& aPath);

	AssetHandle GetAnimation(const std::string& aPath);
	AssetHandle GetAnimationRelative(const std::string& aBase, const std::string& aPath);

	void AssumeOwnershipOfCustomAsset(Asset* aCustomAsset);

private:

	AssetHandle GetTextureInternal(const std::string& aPath, bool aFailSilenty);
	AssetHandle GetJSONInternal(const std::string& aPath);
	AssetHandle GetAnimationInternal(const std::string& aPath);

	Tools::FileWatcher myFileWatcher;

	std::string myBaseFolder;

	ID3D11Device* myDevice = nullptr;
	ModelLoader* myModelLoader = nullptr;
	TextureLoader* myTextureLoader = nullptr;
	ShaderCompiler* myShaderCompiler = nullptr;

	Asset* myErrorTexture = nullptr;

	std::unordered_map<std::string, Asset*> myCachedModels;
	std::unordered_map<std::string, Asset*> myCachedSkyboxes;
	std::unordered_map<std::string, Asset*> myCachedTextures;
	std::unordered_map<std::string, Asset*> myCachedCubeTextures;
	std::unordered_map<std::string, Asset*> myCachedJSON;
	std::unordered_map<std::string, Asset*> myCachedfonts;
	std::unordered_map<std::string, Asset*> myCachedLevels;
	std::unordered_map<std::string, Asset*> myCachedNavMeshes;
	std::unordered_map<std::string, Asset*> myCachedAnimations;

	std::unordered_map<std::string, std::unordered_map<ShaderFlags,Asset*>> myCachedPixelShaders;
	std::unordered_map<std::string, std::unordered_map<ShaderFlags,Asset*>> myCachedVertexShaders;
	std::unordered_map<std::string, std::unordered_map<ShaderFlags,Asset*>> myCachedGeometryShaders;

	std::vector<Asset*> myCustomAssets;
};

