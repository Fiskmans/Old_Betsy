#pragma once

#include "Texture.h"
#include "DirectX11Framework.h"
#include "ModelInstance.h"
#include "Model.h"

class MarchingSquares
{
public:
	MarchingSquares() = default;
	~MarchingSquares();

	bool Init(DirectX11Framework* aFamework);
	void Update();

	void DoMovement(float aDt);

	void GenerateGeometryData();
	void GenerateGeometry();

	void SetupGraphicsResources();

#if USEIMGUI
	void Imgui();
#endif


private:
	const float myHeight = 0_m;
	const float mySize = 64_m;

	const CommonUtilities::Vector2<unsigned int> myResolution {16,16};

	void AddRandomOrb();
	struct Orb
	{
		CommonUtilities::Vector2<float> myPosition;
		CommonUtilities::Vector2<float> mySpeed;

		float mySize;
	};

	float myTheshold = 1.f;
	float mySpeed = 0.1f;
	bool myMove = true;
	float myLastTimeStamp;
	std::vector<Orb> myOrbs;

	UpdatableTexture* myTexture = nullptr;
	Asset* myTextureAsset = nullptr;
	Model::ModelData* myModelData = nullptr;
	DirectX11Framework* myFamework = nullptr;
	ModelAsset* myModelAsset = nullptr;
	AssetHandle myModelHandle;
	ModelInstance* myModelInstance = nullptr;

	struct Vertex
	{
		V4F myPosition;
		V4F myNormal;
		V4F myTangent;
		V4F myBiTangent;
		CommonUtilities::Vector2<float> myUV;
	};

	struct Tri
	{
		uint32_t myA;
		uint32_t myB;
		uint32_t myC;
	};
};

