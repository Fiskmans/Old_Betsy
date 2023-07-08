#ifndef ENGINE_ASSETS_ASSET_H
#define ENGINE_ASSETS_ASSET_H

#include "engine/graphics/Texture.h"
#include "engine/graphics/GBuffer.h"

#include "tools/JSON.h"
#include "tools/Logger.h"

#include <vector>
#include <string>
#include <atomic>
#include <typeinfo>
#include <typeindex>
#include <concepts>

#define NOMINMAX
#include <WinSock2.h>
#include <d3d11.h>


//#include "ShaderFlags.h"
//#include "SpriteFontInclude.h"
//#include "LevelLoader.h"
//#include "FileWatcher.h"

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
		inline bool Is() const { return dynamic_cast<const AssetType*>(this); }

	private:
		friend class AssetManager;
		friend class AssetHandleBase;

		void IncRefCount();
		bool DecRefCount();

		std::atomic<int32_t> myRefCount;
		std::atomic<bool> myIsLoaded;

		std::string myFileName;
		//tools::FileWatcherUniqueID myFileHandle;
	};


	class AssetHandleBase
	{
	public:
		AssetHandleBase(Asset* aAsset = nullptr);
		virtual ~AssetHandleBase();

		AssetHandleBase(const AssetHandleBase& aOther);

		AssetHandleBase& operator=(const AssetHandleBase& aOther);

		bool IsValid() const;
		bool IsLoaded() const;

		template<std::derived_from<Asset> AssetType>
		inline bool Is() const { return myAsset->Is<AssetType>(); }

	protected:
		template<std::derived_from<Asset> BaseAsset>
		friend class AssetHandle;

		Asset* myAsset = nullptr;
	};

	template<std::derived_from<Asset> BaseAsset>
	class AssetHandle : public AssetHandleBase
	{
	public:
		AssetHandle(AssetHandleBase& aOther) : AssetHandle(aOther.myAsset) {}
		AssetHandle(Asset* aAsset = nullptr) : AssetHandleBase(aAsset) 
		{
			if (aAsset && !aAsset->Is<BaseAsset>())
				LOG_SYS_ERROR("Handle created with wrong assettype");
		}


		const BaseAsset& Access() const 
		{
			assert(myAsset);
			return *reinterpret_cast<BaseAsset*>(myAsset); 
		}
	};

	class ModelAsset
		: public Asset
	{
	public:
		ModelAsset(Model* aModel, const std::string& aFilePath);

		Model* myModel;
		std::string myFilePath;
	};

	class TextureAsset
		: public Asset
	{
	public:
		TextureAsset(ID3D11ShaderResourceView* aTexture);
		~TextureAsset();

		ID3D11ShaderResourceView* myTexture;
	protected:
		bool myOwns = true;
	};

	class DepthTextureAsset
		: public Asset
	{
	public:
		DepthTextureAsset(graphics::DepthTexture&& aTexture);

		graphics::DepthTexture myTexture;
	};

	class GBufferAsset
		: public Asset
	{
	public:
		GBufferAsset(graphics::GBuffer&& aGBuffer);

		graphics::GBuffer myGBuffer;
	};

	class DrawableTextureAsset final
		: public TextureAsset
	{
	public:
		DrawableTextureAsset(graphics::Texture&& aTexture);
		graphics::Texture myDrawableTexture;
	};

	class PixelShaderAsset final
		: public Asset
	{
	public:
		PixelShaderAsset(ID3D11PixelShader* aShader);
		~PixelShaderAsset();

		ID3D11PixelShader* myShader;
	};

	class VertexShaderAsset final
		: public Asset
	{
	public:
		VertexShaderAsset(ID3D11VertexShader* aShader, const std::vector<char>& aBlob);
		~VertexShaderAsset();

		ID3D11VertexShader* myShader;
		std::vector<char> myBlob;
	};

	class GeometryShaderAsset final
		: public Asset
	{
	public:
		GeometryShaderAsset(ID3D11GeometryShader* aShader);
		~GeometryShaderAsset();

		ID3D11GeometryShader* myShader;
	};

	class JsonAsset
		: public Asset
	{
	public:
		JsonAsset(const std::string& aPath);

		fisk::tools::Json myObject;
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