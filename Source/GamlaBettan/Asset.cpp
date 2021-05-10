#include "pch.h"
#include "Asset.h"

#include "Model.h"
#include "Skybox.h"

Asset::Asset(AssetType aType)
{
	myType = aType;
	myRefCount = 0;
	myIsLoaded = false;
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
	myAsset->DecRefCount();
	myAsset = aOther.myAsset;
	myAsset->IncRefCount();
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

bool AssetHandle::IsLoaded()
{
	return myAsset->myIsLoaded;
}

Asset::AssetType AssetHandle::GetType()
{
	return myAsset->myType;
}

ModelInstance* AssetHandle::InstansiateModel()
{
	return new ModelInstance(*this);
}

Skybox* AssetHandle::InstansiateSkybox()
{
	return new Skybox(*this);
}

Model* AssetHandle::GetAsModel()
{
	if (myAsset->myType != Asset::AssetType::Model) { throw std::exception("GetAsModel() on non model asset"); }
	return reinterpret_cast<ModelAsset*>(myAsset)->myModel;
}

ModelAsset::ModelAsset(Model* aModel)
	: Asset(AssetType::Model)
{
	myModel = aModel;
}

SkyboxAsset::SkyboxAsset(Skybox* aSkyBox)
	: Asset(AssetType::SkyBox)
{
	mySkybox = aSkyBox;
}
