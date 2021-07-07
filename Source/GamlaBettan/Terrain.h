#pragma once
#include "GamlaBettan\Model.h"
#include "Tools\Handover.h"

class Terrain : public CommonUtilities::Singleton<Terrain>
{
public:
	Terrain();

	void Init(DirectX11Framework* aFramework);
	void Update();

#if USEIMGUI
	void Imgui();
#endif

private:

	static const size_t MaxTriCount = 1 << 20; // ~13mb of graphics memory

	void FromFile(const std::string& aFilePath);

	void FromFBX(const std::string aFilePath);
	void FromTER(const std::string aFilePath);

	void ToFile(const std::string& aFilePath);

	void GenerateVertexInfo();

	void Setup();
	void SetupGraphicsResources();
	void SetupAllNormals();
	void SetupNormals(UINT aVertexThatChanged);
	void SetupNormals(std::vector<UINT> aVertexesAffected);
	float Area(UINT aTri);
	float Triangleness(UINT aTri);

	void TestMesh();

	void MoveVertex(UINT aIndex, V3F aTargetPosition);
	void Tesselate(UINT aTri);
	void SwapVertex(UINT aTri, UINT aOriginal, UINT aTarget);
	UINT XorAllVerticies(UINT aTri);
	UINT AddVertex(V3F aPosition);
	UINT AddTri(UINT aA, UINT aB, UINT aC);

	bool OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize);
	
	struct Vertex
	{
		V3F myPosition;
		V3F myNormal;
	};

	struct VertexInfo
	{
		struct Edge
		{
			UINT myOtherVertex;
			UINT myTri; //The tri that has this vertex followed by myOtherVertex going clockwise around it
		};

		std::vector<Edge> myEdges;
	};

	struct Tri
	{
		UINT myA;
		UINT myB;
		UINT myC;
	};

	DirectX11Framework* myFramework;

	std::vector<Vertex> myVertexes;
	std::vector<VertexInfo> myVertexInfo;

	std::vector<Tri> myTris;
	
	float myTotalVolumeDiscrepancy;

	Model::ModelData* myModelData;
	Asset* myModelAsset;
	AssetHandle myModelHandle;
	ModelInstance* myModelInstance;


};

