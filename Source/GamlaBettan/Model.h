#pragma once

#pragma warning(push)
#pragma warning(disable : 26812)
#include <d3dcommon.h> // D3D_Primative_Topology enum + extra
#include <dxgiformat.h> // DXGI_FORMAT enum
#pragma warning(pop)

#include <FileWatcher.h>
#include <vector>
#include <AABB3D.hpp>

#include "Macros.h"
#include <MetricValue.h>

#include "AnimationData.h"

#if USEIMGUI
#include <map>
#endif // !_RETAIL


struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
class Texture;

class ModelLoader;


class Model
{
public:
	struct LodLevel
	{
		ID3D11Buffer* myIndexBuffer = nullptr;
		ID3D11Buffer** myVertexBuffer = nullptr;
		UINT myVertexBufferCount = 1;
		UINT myNumberOfIndexes = 0;
	};

	struct CModelData
	{
		bool myForceForward = false;
		bool myIsEffect = false;

		size_t myshaderTypeFlags;
		UINT myStride = 0;
		UINT myOffset = 0;

#pragma warning(push)
#pragma warning(disable : 26812)
		D3D_PRIMITIVE_TOPOLOGY myPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		DXGI_FORMAT myIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
#pragma warning(pop)

		Model::LodLevel* myLodLevels[8] = { nullptr };
		class PixelShader* myPixelShader = nullptr;
		class VertexShader* myVertexShader = nullptr;
		ID3D11InputLayout* myInputLayout = nullptr;

		Texture* myTextures[3] = { nullptr ,nullptr, nullptr};

		std::vector<std::string> myAnimations;
		std::string myFilePath;
	};

	std::vector<CommonUtilities::AABB3D<float>> myCollisions;
	std::vector<BoneInfo> myBoneData;
	std::unordered_map<std::string, unsigned int> myBoneNameLookup;

	bool myIsMissNamed = false;

	float myTemporaryBlendValue = 0;

	Model();
	~Model();
	
	void Init(const CModelData& data, ModelLoader* aLoader, const std::string& aPixelShaderFileName, const std::string& aVertexShaderFileName, const std::string& aFilePath = "", const std::string& aFriendlyName = "");

	void ResetAndRelease();

	CModelData* GetModelData();

	static void FlushChanges();

	std::string GetFriendlyName();

	bool inline ShouldRender() { return myIsLoaded && myShouldRender; };
	bool inline ShouldLoad() { return myShouldLoad; };

	void QueueForLoad() { myShouldLoad = true; };

	LodLevel* GetOptimalLodLevel(float aDistanceFromCameraSqr);
	//Will always take care of aLodLevel
	void ApplyLodLevel(LodLevel* aLodLevel,size_t aLevel,float aSize = 0);

#if USEIMGUI
	virtual void ImGuiNode(std::map<std::string, std::vector<std::string>>& aFiles);
#endif // !_RETAIL

	float GetGraphicSize();

private:
#if USEIMGUI
	bool myForceLodLevel = false;
	int myForcedLodLevel = 0;
#endif // !_RETAIL

	GAMEMETRIC(float, myLodLevelDistance, DISTANCEPERLODLEVEL, 2000.f);

	float myGraphicSize = 1;
	bool myShouldLoad = true;
	bool myIsLoaded = false;
	bool myShouldRender = true;
	ModelLoader* myLoader;

	bool ReloadPixelShader(const std::string& aFilePath);
	bool ReloadVertexShader(const std::string& aFilePath);

	CModelData myModelData;

#if USEFILEWATHCER
	static Tools::FileWatcher ourFileWatcher;
	Tools::FileWatcher::UniqueID myPSFileHandle;
	Tools::FileWatcher::UniqueID myVSFileHandle;
#endif

	std::string myFriendlyName;
	std::string myFilePath;
};
