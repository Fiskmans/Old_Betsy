#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include "tools/Singleton.h"
#include "tools/MathVector.h"

#include "engine/assets/Asset.h"
#include "engine/assets/ShaderFlags.h"

#include "engine/assets/ShaderCompiler.h"
#include "engine/assets/TextureLoader.h"
#include "engine/assets/ModelLoader.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace engine
{
	class AssetManager
		: public fisk::tools::Singleton<AssetManager>
	{
	public:
		AssetManager();

		void Init(const std::string& aBaseFolder, const std::string& aBakeFolder);

		void Preload();

		AssetHandle<TextureAsset> GetTexture(const std::string& aPath, bool aFailSilent = false);
		AssetHandle<TextureAsset> GetTextureRelative(const std::string& aBase, const std::string& aPath, bool aFailSilenty = false);
		AssetHandle<DrawableTextureAsset> MakeTexture(const tools::V2ui& aResolution, DXGI_FORMAT aFormat);
		AssetHandle<DepthTextureAsset> MakeDepthTexture(const tools::V2ui& aResolution);
		AssetHandle<GBufferAsset> MakeGBuffer(const tools::V2ui& aResolution);
	
		AssetHandle<TextureAsset> GetCubeTexture(const std::string& aPath);
		AssetHandle<ModelAsset> GetModel(const std::string& aPath);

		AssetHandle<PixelShaderAsset> GetPixelShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());
		AssetHandle<VertexShaderAsset> GetVertexShader(const std::string& aPath, ShaderFlags aFlags = ShaderFlags());

		AssetHandle<JsonAsset> GetJSONRelative(const std::string& aBase, const std::string& aPath);

		void AssumeOwnershipOfCustomAsset(Asset* aCustomAsset);

		void ImGui();
		void Imguicontent();

	private:

		AssetHandle<TextureAsset> GetTextureInternal(const std::string& aPath, bool aFailSilenty);
		AssetHandle<JsonAsset> GetJSONInternal(const std::string& aPath);

		size_t myCustomTextureCounter;

		std::string myBaseFolder;

		std::unique_ptr<assets::ModelLoader> myModelLoader;
		std::unique_ptr<assets::TextureLoader> myTextureLoader;
		std::unique_ptr<assets::ShaderCompiler> myShaderCompiler;

		Asset* myErrorTexture = nullptr;

		std::unordered_map<std::string, AssetHandle<ModelAsset>> myCachedModels;
		std::unordered_map<std::string, AssetHandle<TextureAsset>> myCachedSkyboxes;
		std::unordered_map<std::string, AssetHandle<TextureAsset>> myCachedTextures;
		std::unordered_map<std::string, AssetHandle<TextureAsset>> myCachedCubeTextures;
		std::unordered_map<std::string, AssetHandle<JsonAsset>> myCachedJSON;

		std::unordered_map<std::string, std::unordered_map<ShaderFlags, AssetHandle<PixelShaderAsset>>> myCachedPixelShaders;
		std::unordered_map<std::string, std::unordered_map<ShaderFlags, AssetHandle<VertexShaderAsset>>> myCachedVertexShaders;
		std::unordered_map<std::string, std::unordered_map<ShaderFlags, AssetHandle<GeometryShaderAsset>>> myCachedGeometryShaders;

		std::vector<Asset*> myCustomAssets;
	};

}

#endif