#include "engine/assets/Asset.h"

//#include "ModelInstance.h"
//#include "TextInstance.h"
//#include "GamlaBettan\NavMeshLoader.h"

namespace engine
{
	Asset::Asset(bool aIsLoaded)
	{
		myRefCount = 0;
		myIsLoaded = aIsLoaded;
	}

	void Asset::IncRefCount()
	{
		myRefCount++;
	}

	bool Asset::DecRefCount()
	{
		myRefCount--;
		return myRefCount == 0;
	}

	AssetHandle::AssetHandle(Asset* aAsset)
		: myAsset(aAsset)
	{
		if (myAsset)
		{
			myAsset->IncRefCount();
		}
	}

	AssetHandle::~AssetHandle()
	{
		if (myAsset)
			if (myAsset->DecRefCount())
				delete myAsset;
	}

	AssetHandle::AssetHandle(const AssetHandle& aOther)
	{
		myAsset = aOther.myAsset;
		if (myAsset)
		{
			myAsset->IncRefCount();
		}
	}

	AssetHandle& AssetHandle::operator=(const AssetHandle& aOther)
	{
		if (myAsset)
		{
			if (myAsset->DecRefCount())
				delete myAsset;
		}
		myAsset = aOther.myAsset;
		if (myAsset)
		{
			myAsset->IncRefCount();
		}

		return *this;
	}

	bool AssetHandle::IsValid() const
	{
		return !!myAsset;
	}

	bool AssetHandle::IsLoaded() const
	{
		return myAsset->myIsLoaded || myAsset->CheckLoaded();
	}

	//ModelInstance* AssetHandle::InstansiateModel() const
	//{
	//	return new ModelInstance(*this);
	//}
	//
	//ModelInstance* AssetHandle::InstansiateSkybox() const
	//{
	//	return new ModelInstance(*this);
	//}
	//
	//TextInstance* AssetHandle::InstansiateText() const
	//{
	//	return new TextInstance(*this);
	//}
	//
	//Model* AssetHandle::GetAsModel() const
	//{
	//	if (myAsset->myType != Asset::AssetType::Model && myAsset->myType != Asset::AssetType::SkyBox) { throw std::exception("GetAsModel() on non model asset"); }
	//	return reinterpret_cast<ModelAsset*>(myAsset)->myModel;
	//}
	//
	//std::string AssetHandle::GetModelPath() const
	//{
	//	if (myAsset->myType != Asset::AssetType::Model && myAsset->myType != Asset::AssetType::SkyBox) { throw std::exception("GetAsModel() on non model asset"); }
	//	return reinterpret_cast<ModelAsset*>(myAsset)->myFilePath;
	//}
	//
	//Model* AssetHandle::GetAsSkybox() const
	//{
	//	if (myAsset->myType != Asset::AssetType::SkyBox) { throw std::exception("GetAsSkybox() on non Skybox asset"); }
	//	return reinterpret_cast<SkyboxAsset*>(myAsset)->mySkybox;
	//}

	ModelAsset::ModelAsset(Model* aModel, const std::string& aFilePath)
	{
		myModel = aModel;
		myFilePath = aFilePath;
	}

	TextureAsset::TextureAsset(ID3D11ShaderResourceView* aTexture)
	{
		myTexture = aTexture;
	}

	TextureAsset::~TextureAsset()
	{
		myTexture->Release();
	}

	DrawableTextureAsset::DrawableTextureAsset(graphics::Texture& aTexture)
		: TextureAsset(aTexture.GetResourceView())
	{
		TextureAsset::myOwns = false;
		myDrawableTexture = aTexture;
	}

	PixelShaderAsset::PixelShaderAsset(ID3D11PixelShader* aShader)
	{
		myShader = aShader;
	}

	PixelShaderAsset::~PixelShaderAsset()
	{
		myShader->Release();
	}

	VertexShaderAsset::VertexShaderAsset(ID3D11VertexShader* aShader, const std::vector<char>& aBlob)
	{
		myShader = aShader;
		myBlob = aBlob;
	}

	VertexShaderAsset::~VertexShaderAsset()
	{
		myShader->Release();
	}

	GeometryShaderAsset::GeometryShaderAsset(ID3D11GeometryShader* aShader)
	{
		myShader = aShader;
	}

	GeometryShaderAsset::~GeometryShaderAsset()
	{
		myShader->Release();
	}

	DrawableTextureAsset::~DrawableTextureAsset()
	{
		myDrawableTexture.Release();
	}

	JSONAsset::JSONAsset(tools::JSONObject* aObject, const std::string& aPath)
	{
		myObject = aObject;
		myPath = aPath;
	}

	JSONAsset::~JSONAsset()
	{
		delete myObject;
	}

	//FontAsset::FontAsset(DirectX::SpriteFont* aFont)
	//	: Asset(AssetType::Font)
	//{
	//	myFont = aFont;
	//}
	//
	//LevelAsset::LevelAsset(std::future<LevelParseResult>&& aLevel)
	//	: Asset(AssetType::Level, false)
	//{
	//	myLoadingLevel = std::move(aLevel);
	//}
	//
	//NavMeshAsset::NavMeshAsset(NavMesh* aNavmesh)
	//	: Asset(AssetType::NavMesh)
	//{
	//	myNavMesh = aNavmesh;
	//}
	//
	//AnimationAsset::AnimationAsset(Animation* aAnimation)
	//	: Asset(AssetType::Animation)
	//{
	//	myAnimation = aAnimation;
	//}
}