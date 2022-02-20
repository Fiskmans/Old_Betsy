#pragma once
#include "GamlaBettan\Model.h"
#include "Tools\Handover.h"
#include "GBPhysX\GBPhysX.h"

class Terrain : public CommonUtilities::Singleton<Terrain>
{
public:
	Terrain(DirectX11Framework* aFramework, GBPhysX* aPhysx, const CommonUtilities::Vector3<size_t>& aResolution, const V3F aSize);

	void Update();

#if USEIMGUI
	void Imgui();
	void ImguiContent();
#endif

	struct DigResult
	{
		size_t myDirt = 0;
		size_t myRock = 0;
		size_t myIron = 0;
		size_t myGold = 0;

		bool Empty() const { return myDirt == 0 && myRock == 0 && myIron == 0 && myGold == 0; }
	};


	DigResult Dig(V3F aPosition, size_t aPower);

private:

	struct TerrainNode
	{
		uint8_t myDirt = 0;
		uint8_t myRock = 0;
		uint8_t myIron = 0;
		uint8_t myGold = 0;

		operator float();
		void AttemptDig(DigResult& aInOutResult, size_t aPower);
		bool Empty() const { return myDirt == 0 && myRock == 0 && myIron == 0 && myGold == 0; }
	};

	std::vector<TerrainNode> myTerrainData;
	CommonUtilities::Vector3<size_t> myResolution;
	CommonUtilities::Vector3<size_t> myIndexing;
	size_t MaxTriCount = 0; // ~13mb of graphics memory
	V3F mySize = {1,1,1};

	struct Vertex
	{
		V3F myPosition;
		V3F myNormal;
		uint32_t mySeed = 0;
	};
	
	void GenerateTerrain();

	void FromFile(const std::string& aFilePath);

	void FromTER(const std::string aFilePath);

	void ToFile(const std::string& aFilePath); 

	void Setup();
	void SetupGraphicsResources();

	bool OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize);
	

	DirectX11Framework* myFramework = nullptr;
	GBPhysX* myPhysx = nullptr;

	//DATA
	std::vector<Vertex> myVertexes;

	Model::ModelData* myModelData = nullptr;
	Asset* myModelAsset = nullptr;
	AssetHandle myModelHandle;
	ModelInstance* myModelInstance = nullptr;

	GBPhysXActor* myActor = nullptr;

	bool myIsDirty = true;

	size_t myMaxStepsPerFrame = 5000;

	float myLastRebake = 0;

	const float myRebakeInterval = 0.16f;
};

