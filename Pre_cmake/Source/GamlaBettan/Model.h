#pragma once

#pragma warning(push)
#pragma warning(disable : 26812)
#include <d3dcommon.h> // D3D_Primative_Topology enum + extra
#include <dxgiformat.h> // DXGI_FORMAT enum
#pragma warning(pop)

#include <MetricValue.h>

#include "AnimationData.h"

#include "CommonUtilities/AABB3D.hpp"

struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

class ModelLoader;

class AssetHandle;


class Model
{
public:

	struct ModelData
	{
		~ModelData();

		ShaderFlags myshaderTypeFlags = ShaderFlags::None;
		UINT myStride = 0;

#pragma warning(push)
#pragma warning(disable : 26812)
		D3D_PRIMITIVE_TOPOLOGY myPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		DXGI_FORMAT myIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
#pragma warning(pop)
		AssetHandle myPixelShader;
		AssetHandle myVertexShader;

		AssetHandle myTextures[3];
		std::vector<AssetHandle> myCustomTextures;

		bool myIsIndexed = true;
		UINT myNumberOfIndexes = 0;
		UINT myNumberOfVertexes = 0;
		ID3D11Buffer* myIndexBuffer = nullptr;
		ID3D11Buffer* myVertexBuffer = nullptr;
		ID3D11InputLayout* myInputLayout = nullptr;

		bool myUseForwardRenderer = false;

		V3F myDiffuseColor = V3F(0.8f, 0.4f, 0.75f);

		M44f myOffset;
	};

	AssetHandle myAnimations;

	std::vector<BoneInfo> myBoneData;
	std::unordered_map<std::string, unsigned int> myBoneNameLookup;

	Model();
	~Model();
	
	void AddModelPart(ModelData* aPart);

	const std::vector<ModelData*> GetModelData();


	void MarkLoaded() { myIsLoaded = true; }
	bool inline ShouldRender() { return myIsLoaded && myShouldRender; };

	bool inline ShouldRenderWithForwardRenderer() 
	{
		for (ModelData* i : myModelData)
		{
			if (i->myUseForwardRenderer)
			{
				return true;
			}
		}
		return false;
	}

#if USEIMGUI
	virtual void ImGuiNode(std::map<std::string, std::vector<std::string>>& aFiles);
#endif // !_RETAIL

	float GetGraphicSize();

	std::string myFilePath;


private:

	GAMEMETRIC(float, myLodLevelDistance, DISTANCEPERLODLEVEL, 2000.f);

	float myGraphicSize = 1;
	bool myIsLoaded = false;
	bool myShouldRender = true;

	std::vector<ModelData*> myModelData;

};
