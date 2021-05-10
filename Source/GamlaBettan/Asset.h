#pragma once

#include <atomic>
#include "Model.h"

#include "ShaderFlags.h"

class ModelInstance;
class Skybox;

class Asset
{
public:
	enum class AssetType
	{
		Model,
		SkyBox,
		Texture,
		PixelShader
	};

	Asset(AssetType aType);

	AssetType myType;

	std::atomic<int32_t> myRefCount;
	std::atomic<bool> myIsLoaded;

	void IncRefCount();
	void DecRefCount();

};


class AssetHandle
{
public:
	AssetHandle(Asset* aAsset = nullptr);
	~AssetHandle();

	AssetHandle(const AssetHandle& aOther);

	AssetHandle& operator=(const AssetHandle& aOther);


	bool IsLoaded();

	Asset::AssetType GetType();
	
	ModelInstance*				InstansiateModel();
	Skybox*						InstansiateSkybox();

	Model*						GetAsModel();
	ID3D11ShaderResourceView*	GetAsShaderResource();

private:
	Asset* myAsset;
};

class ModelAsset
	: public Asset
{
public:
	ModelAsset(Model* aModel);

	Model* myModel;
};

class SkyboxAsset
	: public Asset
{
public:
	SkyboxAsset(Skybox* aSkyBox);

	Skybox* mySkybox;
};