#pragma once

#include "ShaderFlags.h"
#include "SpriteFontInclude.h"
#include "LevelLoader.h"
#include "FileWatcher.h"

struct ID3D11ShaderResourceView;
class ModelInstance;
class Model;
class TextInstance;
struct NavMesh;
class Animation;

class Asset
{
public:
	enum class AssetType
	{
		Invalid,
		Model,
		SkyBox,
		Texture,
		PixelShader,
		VertexShader,
		GeometryShader,
		JSON,
		Font,
		Level,
		NavMesh,
		Animation
	};

	Asset(AssetType aType,bool aIsLoaded = true);

	bool CheckLoaded();

	AssetType myType;


	void IncRefCount();
	void DecRefCount();
private:
	std::atomic<int32_t> myRefCount;
	std::atomic<bool> myIsLoaded;
	friend class AssetManager;
	friend class AssetHandle;

	std::string myFileName;
	Tools::FileWatcherUniqueID myFileHandle;
};


class AssetHandle
{
public:
	AssetHandle(Asset* aAsset = nullptr);
	~AssetHandle();

	AssetHandle(const AssetHandle& aOther);

	AssetHandle& operator=(const AssetHandle& aOther);


	bool IsValid() const;
	bool IsLoaded() const;

	Asset::AssetType GetType() const;

	ModelInstance* InstansiateModel() const;
	ModelInstance* InstansiateSkybox() const;
	TextInstance* InstansiateText() const;

	Model* GetAsModel() const;
	std::string GetModelPath() const;
	Model* GetAsSkybox() const;
	ID3D11ShaderResourceView* GetAsTexture() const;

	ID3D11PixelShader* GetAsPixelShader() const;
	ID3D11VertexShader* GetAsVertexShader() const;
	ID3D11GeometryShader* GetAsGeometryShader() const;

	std::vector<char>& GetVertexShaderblob() const;

	FiskJSON::Object& GetAsJSON() const;
	std::string GetJSONFilePath() const;

	DirectX::SpriteFont* GetAsFont() const;

	LevelParseResult& GetAsLevel() const;

	NavMesh* GetAsNavMesh() const;

	Animation* GetAsAnimation() const;

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

class PixelShaderAsset
	: public Asset
{
public:
	PixelShaderAsset(ID3D11PixelShader* aShader);

	ID3D11PixelShader* myShader;
};

class VertexShaderAsset
	: public Asset
{
public:
	VertexShaderAsset(ID3D11VertexShader* aShader,const std::vector<char>& aBlob);

	ID3D11VertexShader* myShader;
	std::vector<char> myBlob;
};

class GeometryShaderAsset
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
	JSONAsset(FiskJSON::Object* aObject,const std::string& aPath);

	FiskJSON::Object* myObject;
	std::string myPath;
};

class FontAsset
	: public Asset
{
public:
	FontAsset(DirectX::SpriteFont* aFont);

	DirectX::SpriteFont* myFont;
};

class LevelAsset
	: public Asset
{
public:
	LevelAsset(std::future<LevelParseResult>&& aLevel);

	LevelParseResult myLevel;
	std::future<LevelParseResult> myLoadingLevel;
};

class NavMeshAsset
	: public Asset
{
public:
	NavMeshAsset(NavMesh* aNavmesh);

	NavMesh* myNavMesh;
};

class AnimationAsset
	: public Asset
{
public:
	AnimationAsset(Animation* aAnimation);

	Animation* myAnimation;
};