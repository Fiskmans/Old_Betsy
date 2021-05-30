#include "pch.h"
#include "Asset.h"

#include "Model.h"
#include "TextInstance.h"

Asset::Asset(AssetType aType,bool aIsLoaded)
{
	myType = aType;
	myRefCount = 0;
	myIsLoaded = aIsLoaded;
}

bool Asset::CheckLoaded()
{
	switch (myType)
	{
	default:
		return true;
	case Asset::AssetType::Level:
		if (reinterpret_cast<LevelAsset*>(this)->myLoadingLevel.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
		{
			reinterpret_cast<LevelAsset*>(this)->myLevel = reinterpret_cast<LevelAsset*>(this)->myLoadingLevel.get();
			myIsLoaded = true;
			return true;
		}
		return false;
	}
}

void Asset::IncRefCount()
{
	myRefCount++;
}

void Asset::DecRefCount()
{
	myRefCount--;
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
	{
		myAsset->DecRefCount();
	}
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
		myAsset->DecRefCount();
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

Asset::AssetType AssetHandle::GetType() const
{
	return myAsset->myType;
}

ModelInstance* AssetHandle::InstansiateModel() const
{
	return new ModelInstance(*this);
}

ModelInstance* AssetHandle::InstansiateSkybox() const
{
	return new ModelInstance(*this);
}

TextInstance* AssetHandle::InstansiateText() const
{
	return new TextInstance(*this);
}

Model* AssetHandle::GetAsModel() const
{
	if (myAsset->myType != Asset::AssetType::Model && myAsset->myType != Asset::AssetType::SkyBox) { throw std::exception("GetAsModel() on non model asset"); }
	return reinterpret_cast<ModelAsset*>(myAsset)->myModel;
}

Model* AssetHandle::GetAsSkybox() const
{
	if (myAsset->myType != Asset::AssetType::SkyBox) { throw std::exception("GetAsSkybox() on non Skybox asset"); }
	return reinterpret_cast<SkyboxAsset*>(myAsset)->mySkybox;
}

ID3D11ShaderResourceView* AssetHandle::GetAsTexture() const
{
	if (myAsset->myType != Asset::AssetType::Texture) { throw std::exception("GetAsTexture() on non texture asset"); }
	return reinterpret_cast<TextureAsset*>(myAsset)->myTexture;
}

ID3D11PixelShader* AssetHandle::GetAsPixelShader() const
{
	if (myAsset->myType != Asset::AssetType::PixelShader) { throw std::exception("GetAsPixxelShader() on non PixelShader asset"); }
	return reinterpret_cast<PixelShaderAsset*>(myAsset)->myShader;
}

ID3D11VertexShader* AssetHandle::GetAsVertexShader() const
{
	if (myAsset->myType != Asset::AssetType::VertexShader) { throw std::exception("GetAsVertexShader() on non VertexShader asset"); }
	return reinterpret_cast<VertexShaderAsset*>(myAsset)->myShader;
}

ID3D11GeometryShader* AssetHandle::GetAsGeometryShader() const
{
	if (myAsset->myType != Asset::AssetType::GeometryShader) { throw std::exception("GetAsGeometryShader() on non geomtryShader asset"); }
	return reinterpret_cast<GeometryShaderAsset*>(myAsset)->myShader;
}

std::vector<char>& AssetHandle::GetVertexShaderblob() const
{
	if (myAsset->myType != Asset::AssetType::VertexShader) { throw std::exception("GetAsVertexShader() on non VertexShader asset"); }
	return reinterpret_cast<VertexShaderAsset*>(myAsset)->myBlob;
}

FiskJSON::Object& AssetHandle::GetAsJSON() const
{
	if (myAsset->myType != Asset::AssetType::JSON) { throw std::exception("GetAsVertexShader() on non JSON asset"); }
	return *reinterpret_cast<JSONAsset*>(myAsset)->myObject;
}

std::string AssetHandle::GetJSONFilePath() const
{
	if (myAsset->myType != Asset::AssetType::JSON) { throw std::exception("GetAsVertexShader() on non VertexShader asset"); }
	return reinterpret_cast<JSONAsset*>(myAsset)->myPath;
}

DirectX::SpriteFont* AssetHandle::GetAsFont() const
{
	if (myAsset->myType != Asset::AssetType::Font) { throw std::exception("GetAsFont() on non Font asset"); }
	return reinterpret_cast<FontAsset*>(myAsset)->myFont;
}

LevelParseResult& AssetHandle::GetAsLevel() const
{
	if (myAsset->myType != Asset::AssetType::Level) { throw std::exception("GetAsLevel() on non Level asset"); }
	return reinterpret_cast<LevelAsset*>(myAsset)->myLevel;
}

NavMesh* AssetHandle::GetAsNavMesh() const
{
	if (myAsset->myType != Asset::AssetType::NavMesh) { throw std::exception("GetAsNavMesh() on non NavMesh asset"); }
	return reinterpret_cast<NavMeshAsset*>(myAsset)->myNavMesh;
}

Animation* AssetHandle::GetAsAnimation() const
{
	if (myAsset->myType != Asset::AssetType::Animation) { throw std::exception("GetAsAnimation() on non Animation asset"); }
	return reinterpret_cast<AnimationAsset*>(myAsset)->myAnimation;
}

ModelAsset::ModelAsset(Model* aModel)
	: Asset(AssetType::Model)
{
	myModel = aModel;
}

SkyboxAsset::SkyboxAsset(Model* aSkyBox)
	: Asset(AssetType::SkyBox)
{
	mySkybox = aSkyBox;
}

TextureAsset::TextureAsset(ID3D11ShaderResourceView* aTexture)
	: Asset(AssetType::Texture)
{
	myTexture = aTexture;
}

PixelShaderAsset::PixelShaderAsset(ID3D11PixelShader* aShader) 
	: Asset(AssetType::PixelShader)
{
	myShader = aShader;
}

VertexShaderAsset::VertexShaderAsset(ID3D11VertexShader* aShader, const std::vector<char>& aBlob)
	: Asset(AssetType::VertexShader)
{
	myShader = aShader;
	myBlob = aBlob;
}

GeometryShaderAsset::GeometryShaderAsset(ID3D11GeometryShader* aShader)
	: Asset(AssetType::GeometryShader)
{
	myShader = aShader;
}

JSONAsset::JSONAsset(FiskJSON::Object* aObject, const std::string& aPath)
	: Asset(AssetType::JSON)
{
	myObject = aObject;
	myPath = aPath;
}

FontAsset::FontAsset(DirectX::SpriteFont* aFont)
	: Asset(AssetType::Font)
{
	myFont = aFont;
}

LevelAsset::LevelAsset(std::future<LevelParseResult>&& aLevel)
	: Asset(AssetType::Level, false)
{
	myLoadingLevel = std::move(aLevel);
}

NavMeshAsset::NavMeshAsset(NavMesh* aNavmesh)
	: Asset(AssetType::NavMesh)
{
	myNavMesh = aNavmesh;
}

AnimationAsset::AnimationAsset(Animation* aAnimation)
	: Asset(AssetType::Animation)
{
	myAnimation = aAnimation;
}