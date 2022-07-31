#ifndef ENGINE_ASSETS_MODEL_H
#define ENGINE_ASSETS_MODEL_H

#include "engine/assets/ShaderFlags.h"
#include "engine/assets/Asset.h"
#include "engine/assets/AnimationData.h"

#include "tools/Matrix4x4.h"

#include <unordered_map>

#include <d3d11.h>

namespace engine
{

	class Model
	{
	public:

		struct ModelData
		{
			~ModelData();

			ShaderFlags myshaderTypeFlags = ShaderFlags::None;
			UINT myStride = 0;

			D3D_PRIMITIVE_TOPOLOGY myPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			DXGI_FORMAT myIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;

			AssetHandle myPixelShader;
			AssetHandle myVertexShader;

			AssetHandle myTextures[3];
			std::vector<AssetHandle> myCustomTextures;
			tools::V4f myDiffuseColor;

			bool myIsIndexed = true;
			UINT myNumberOfIndexes = 0;
			UINT myNumberOfVertexes = 0;
			ID3D11Buffer* myIndexBuffer = nullptr;
			ID3D11Buffer* myVertexBuffer = nullptr;
			ID3D11InputLayout* myInputLayout = nullptr;

			bool myUseForwardRenderer = false;

			tools::M44f myOffset;
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

		float GetSize();

		std::string myFilePath;


	private:

		float myGraphicSize = 1;
		bool myIsLoaded = false;
		bool myShouldRender = true;

		std::vector<ModelData*> myModelData;

	};

}

#endif