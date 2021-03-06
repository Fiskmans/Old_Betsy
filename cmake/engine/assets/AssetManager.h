#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include "tools/Singleton.h"
#include "tools/MathVector.h"

#include "engine/assets/Asset.h"
#include "engine/assets/ShaderFlags.h"

#include "engine/assets/ShaderCompiler.h"
#include "engine/assets/TextureLoader.h"

//#include "ModelLoader.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace engine
{
	class AssetManager
		: public tools::Singleton<AssetManager>
	{
	public:
		AssetManager();

		void Init(const std::string& aBaseFolder, const std::string& aBakeFolder);

		void Preload();

		void FlushChanges();

		AssetHandle GetTexture(const std::string& aPath, bool aFailSilent = false);
		AssetHandle MakeTexture(const tools::V2ui& aResolution, DXGI_FORMAT aFormat);
		//AssetHandle GetTextureRelative(const std::string& aBase, const std::string& aPath, bool aFailSilenty = false);
	
		//AssetHandle GetCubeTexture(const std::string& aPath);
		//AssetHandle GetModel(const std::string& aPath);
		//AssetHandle GetSkybox(const std::string& aPath);

		AssetHandle GetPixelShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());
		AssetHandle GetVertexShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());
		//AssetHandle GetGeometryShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());

		//AssetHandle GetPerlinTexture(tools::V2ui aSize, tools::V2f aScale, unsigned int aSeed);

		//AssetHandle GetJSON(const std::string& aPath);
		//AssetHandle GetJSONRelative(const std::string& aBase, const std::string& aPath);

		//AssetHandle GetFont(const std::string& aPath);

		//AssetHandle GetLevel(const std::string& aPath);

		//AssetHandle GetNavMesh(const std::string& aPath);

		//AssetHandle GetAnimation(const std::string& aPath);
		//AssetHandle GetAnimationRelative(const std::string& aBase, const std::string& aPath);

		void AssumeOwnershipOfCustomAsset(Asset* aCustomAsset);

	private:

		AssetHandle GetTextureInternal(const std::string& aPath, bool aFailSilenty);
		//AssetHandle GetJSONInternal(const std::string& aPath);
		//AssetHandle GetAnimationInternal(const std::string& aPath);

		size_t myCustomTextureCounter;

		//Tools::FileWatcher myFileWatcher;

		std::string myBaseFolder;

	//	std::unique_ptr<ModelLoader> myModelLoader;
		std::unique_ptr<assets::TextureLoader> myTextureLoader;
		std::unique_ptr<assets::ShaderCompiler> myShaderCompiler;

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

		std::unordered_map<std::string, std::unordered_map<ShaderFlags, Asset*>> myCachedPixelShaders;
		std::unordered_map<std::string, std::unordered_map<ShaderFlags, Asset*>> myCachedVertexShaders;
		std::unordered_map<std::string, std::unordered_map<ShaderFlags, Asset*>> myCachedGeometryShaders;

		std::vector<Asset*> myCustomAssets;
	};

}

#endif