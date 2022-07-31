#ifndef ENGINE_ASSETS_ASSET_H
#define ENGINE_ASSETS_ASSET_H

#include "engine/graphics/Texture.h"

#include "tools/JSON.h"

#include <vector>
#include <string>
#include <atomic>
#include <typeinfo>
#include <typeindex>
#include <concepts>

#include <d3d11.h>


//#include "ShaderFlags.h"
//#include "SpriteFontInclude.h"
//#include "LevelLoader.h"
//#include "FileWatcher.h"

//class ModelInstance;
//class TextInstance;
//struct NavMesh;
//class Animation;

namespace engine
{
	class Model;

	class Asset
	{
	public:

		Asset(bool aIsLoaded = true);
		virtual ~Asset() = default;

		inline virtual bool CheckLoaded() { return true; }

		template<std::derived_from<Asset> AssetType>
		inline bool Is() const { return typeid(AssetType) == myType; }

	private:
		friend class AssetManager;
		friend class AssetHandle;

		void IncRefCount();
		void DecRefCount();

		const std::type_info& myType;
		std::atomic<int32_t> myRefCount;
		std::atomic<bool> myIsLoaded;

		std::string myFileName;
		//tools::FileWatcherUniqueID myFileHandle;
	};


	class AssetHandle
	{
	public:
		AssetHandle(Asset* aAsset = nullptr);
		virtual ~AssetHandle();

		AssetHandle(const AssetHandle& aOther);

		AssetHandle& operator=(const AssetHandle& aOther);

		bool IsValid() const;
		bool IsLoaded() const;

		template<std::derived_from<Asset> AssetType>
		inline bool Is() const { return myAsset->Is<AssetType>(); }

		template<std::derived_from<Asset> AssetType>
		inline const AssetType& Get() const { return *reinterpret_cast<const AssetType*>(myAsset); }

	private:
		Asset* myAsset = nullptr;
	};

	class ModelAsset
		: public Asset
	{
	public:
		ModelAsset(Model* aModel, const std::string& aFilePath);

		Model* myModel;
		std::string myFilePath;
	};

	class SkyboxAsset
		: public Asset
	{
	public:
		SkyboxAsset(Model* aSkyBox);

		Model* mySkybox;
	};

	class TextureAsset
		: public Asset
	{
	public:
		TextureAsset(ID3D11ShaderResourceView* aTexture);

		ID3D11ShaderResourceView* myTexture;
	};

	class DrawableTextureAsset final
		: public TextureAsset
	{
	public:
		DrawableTextureAsset(graphics::Texture& aTexture);

		~DrawableTextureAsset();

		graphics::Texture myDrawableTexture;
	};

	class PixelShaderAsset final
		: public Asset
	{
	public:
		PixelShaderAsset(ID3D11PixelShader* aShader);

		ID3D11PixelShader* myShader;
	};

	class VertexShaderAsset final
		: public Asset
	{
	public:
		VertexShaderAsset(ID3D11VertexShader* aShader, const std::vector<char>& aBlob);

		ID3D11VertexShader* myShader;
		std::vector<char> myBlob;
	};

	class GeometryShaderAsset final
		: public Asset
	{
	public:
		GeometryShaderAsset(ID3D11GeometryShader* aShader);

		ID3D11GeometryShader* myShader;
	};

	class JSONAsset
		: public Asset
	{
	public:
		JSONAsset(tools::JSONObject* aObject, const std::string& aPath);

		tools::JSONObject* myObject;
		std::string myPath;
	};

	//class FontAsset
	//	: public Asset
	//{
	//public:
	//	FontAsset(DirectX::SpriteFont* aFont);
	//
	//	DirectX::SpriteFont* myFont;
	//};
	//
	//class LevelAsset
	//	: public Asset
	//{
	//public:
	//	LevelAsset(std::future<LevelParseResult>&& aLevel);
	//
	//	LevelParseResult myLevel;
	//	std::future<LevelParseResult> myLoadingLevel;
	//if (reinterpret_cast<LevelAsset*>(this)->myLoadingLevel.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	//{
	//	reinterpret_cast<LevelAsset*>(this)->myLevel = reinterpret_cast<LevelAsset*>(this)->myLoadingLevel.get();
	//	myIsLoaded = true;
	//	return true;
	//}
	//};
	//
	//class NavMeshAsset
	//	: public Asset
	//{
	//public:
	//	NavMeshAsset(NavMesh* aNavmesh);
	//
	//	NavMesh* myNavMesh;
	//};
	//
	//class AnimationAsset
	//	: public Asset
	//{
	//public:
	//	AnimationAsset(Animation* aAnimation);
	//
	//	Animation* myAnimation;
	//};

}

#endif