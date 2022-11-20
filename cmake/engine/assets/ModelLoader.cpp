#include "engine/assets/ModelLoader.h"

#include "engine/assets/ShaderTypes.h"
#include "engine/assets/AnimationData.h"
#include "engine/assets/ShaderFlags.h"
#include "engine/assets/Model.h"
#include "engine/assets/AssetManager.h"

#include "engine/graphics/GraphicEngine.h"

#include "logger/Logger.h"

#include "tools/Functors.h"
#include "tools/FileHelpers.h"
#include "tools/TimeHelper.h"

#include "common/Macros.h"

#include <functional>
#include <stack>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

namespace engine::assets
{
	namespace modelloader_helpers
	{
		tools::M44f ConvertMatrix(aiMatrix4x4 aMatrix)
		{
			return tools::M44f(
				aMatrix.a1, aMatrix.a2, aMatrix.a3, aMatrix.a4,
				aMatrix.b1, aMatrix.b2, aMatrix.b3, aMatrix.b4,
				aMatrix.c1, aMatrix.c2, aMatrix.c3, aMatrix.c4,
				aMatrix.d1, aMatrix.d2, aMatrix.d3, aMatrix.d4);
		}
	}


	struct Vertex
	{
		float x, y, z, w;
		float nx, ny, nz, nw;
		float tx, ty, tz, tw;
		float btx, bty, btz, btw;
		float u, v;
	};

	bool LoadVerticies(const aiMesh* fbxMesh, char** aVertexBuffer, size_t* aVertexCount, size_t aFlags, std::unordered_map<std::string, unsigned int>& aBoneIndexMap, std::vector<BoneInfo>& aBoneInfo)
	{
		LOG_SYS_VERBOSE("Loading model with flags: " + ShaderTypes::PostfixFromFlags(aFlags));
		if (!fbxMesh->HasPositions())
		{
			LOG_ERROR("Model does not have positions");
			return false;
		}
		if (!fbxMesh->HasNormals())
		{
			LOG_ERROR("Model does not have normals");
			return false;
		}

		ShaderTypes::Offsets offsets = ShaderTypes::OffsetsFromFlags(aFlags);

		float one = 1.0f;
		float zero = 0.0f;

		char* buffer = new char[(*aVertexCount + fbxMesh->mNumVertices) * offsets.size];
		memcpy(buffer, *aVertexBuffer, *aVertexCount * offsets.size);

		delete[] * aVertexBuffer;
		*aVertexBuffer = buffer;

		*aVertexCount += fbxMesh->mNumVertices;

		std::vector<VertexBoneData> collectedBoneData;
		if (aFlags & ShaderFlags::HasBones)
		{
			if (fbxMesh->HasBones())
			{
				collectedBoneData.resize(fbxMesh->mNumVertices);

				unsigned int BoneIndex = 0;
				for (unsigned int i = 0; i < fbxMesh->mNumBones; i++)
				{
					std::string BoneName(fbxMesh->mBones[i]->mName.C_Str());
					if (aBoneIndexMap.find(BoneName) == aBoneIndexMap.end())
					{
						BoneIndex = static_cast<unsigned int>(aBoneIndexMap.size());
						BoneInfo bi;
						aBoneInfo.push_back(bi);


						tools::M44f NodeTransformation = modelloader_helpers::ConvertMatrix(fbxMesh->mBones[i]->mOffsetMatrix);

						aBoneInfo[BoneIndex].BoneOffset = NodeTransformation;
						aBoneInfo[BoneIndex].myName = BoneName;
						aBoneIndexMap[BoneName] = BoneIndex;
					}
					else
					{
						BoneIndex = aBoneIndexMap[BoneName];
					}

					for (unsigned int j = 0; j < fbxMesh->mBones[i]->mNumWeights; j++)
					{
						unsigned int VertexID = fbxMesh->mBones[i]->mWeights[j].mVertexId;
						float Weight = fbxMesh->mBones[i]->mWeights[j].mWeight;
						collectedBoneData[VertexID].AddBoneData(BoneIndex, Weight);
					}
				}
			}
		}


#define NUMTHREADS 1

		auto func = [&](int offset)
		{

			for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
			{
				memcpy(buffer + (offsets.size * i + offsets.position), &(fbxMesh->mVertices[i]), sizeof(float) * 3);
				memcpy(buffer + (offsets.size * i + offsets.position + 3 * sizeof(float)), &one, sizeof(float) * 1);

				memcpy(buffer + (offsets.size * i + offsets.normal), &fbxMesh->mNormals[i], sizeof(float) * 3);
				memcpy(buffer + (offsets.size * i + offsets.normal + 3 * sizeof(float)), &zero, sizeof(float) * 1);
			}
			if (fbxMesh->HasTangentsAndBitangents())
			{
				for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
				{
					memcpy(buffer + (offsets.size * i + offsets.tangent), &fbxMesh->mTangents[i], sizeof(float) * 3);
					memcpy(buffer + (offsets.size * i + offsets.tangent + 3 * sizeof(float)), &zero, sizeof(float) * 1);
					memcpy(buffer + (offsets.size * i + offsets.bitanget), &fbxMesh->mBitangents[i], sizeof(float) * 3);
					memcpy(buffer + (offsets.size * i + offsets.bitanget + 3 * sizeof(float)), &zero, sizeof(float) * 1);
				}
			}
			else
			{
				for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
				{
					tools::V4f* normal = reinterpret_cast<tools::V4f*>(buffer + (offsets.size * i + offsets.normal));
					tools::V4f* tangent = reinterpret_cast<tools::V4f*>(buffer + (offsets.size * i + offsets.tangent));
					tools::V4f* biTangent = reinterpret_cast<tools::V4f*>(buffer + (offsets.size * i + offsets.bitanget));
					if ((*normal) == tools::V4f(0, 1, 0, 1))
					{
						*tangent = tools::V4f(1, 0, 0, 1);
						*biTangent = tools::V4f(0, 0, 1, 1);
					}
					else
					{
						*tangent = tools::V3f(*normal).Cross({ 0, 1, 0 }).GetNormalized().Extend(1);
						*biTangent = tools::V3f(*normal).Cross(*tangent).GetNormalized().Extend(1);
					}
				}
			}
			if (aFlags & ShaderFlags::HasUvSets)
			{
				size_t numberOfUvSets = ShaderTypes::BonePerVertexCountFromFlags(aFlags);
				for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
				{
					for (size_t uvIndex = 0; uvIndex < numberOfUvSets; uvIndex++)
					{
						memcpy(buffer + (offsets.size * i + offsets.uv + uvIndex * sizeof(float) * 2), &fbxMesh->mTextureCoords[uvIndex][i], sizeof(float) * 2);
					}
				}
			}
			if (aFlags & ShaderFlags::HasVertexColors)
			{
				for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
				{
					memcpy(buffer + (offsets.size * i + offsets.vertexcolor), &fbxMesh->mColors[0][i], sizeof(float) * 4);
				}
			}
			if (aFlags & ShaderFlags::HasBones)
			{
				size_t numberofBones = ShaderTypes::BonePerVertexCountFromFlags(aFlags);
				for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
				{
					memcpy(buffer + (offsets.size * i + offsets.bones), &collectedBoneData[i].IDs, sizeof(UINT) * numberofBones);
					memcpy(buffer + (offsets.size * i + offsets.boneweights), &collectedBoneData[i].Weights, sizeof(float) * numberofBones);
				}
			}
		};

#if NUMTHREADS > 1

		std::thread threadPool[NUMTHREADS];

		for (size_t i = 0; i < NUMTHREADS; ++i)
		{
			threadPool[i] = std::thread(func, i);
		}

		for (size_t i = 0; i < NUMTHREADS; ++i)
		{
			threadPool[i].join();
		}
#else
		func(0);
#endif

		return true;
	}

	ModelLoader::ModelLoader(const std::string& aDefaultPixelShader)
	{
		myDefaultPixelShader = aDefaultPixelShader;
		myWorkHorse = std::thread(std::bind(&ModelLoader::LoadLoop, this));
	}

	ModelLoader::~ModelLoader()
	{
		myIsRunning = false;
		myWorkHorse.join();
	}

	void ModelLoader::LoadLoop()
	{
		NAMETHREAD(L"ModelLoader-Workhorse");

		while (myIsRunning)
		{
			LoadRequest package;
			package.myEmpty = true;
			for (size_t i = 0; i < myHandoverSlots; i++)
			{
				if (!myHandovers[i].myEmpty)
				{
					package = myHandovers[i];
					myHandovers[i].myEmpty = true;
					break;
				}
			}
			if (package.myEmpty)
			{
				std::this_thread::yield();
				continue;
			}

			LoadModel_Internal(package.myModel, package.myFilePath);
		}
	}

	void ModelLoader::LoadAttributes(const aiNode* aNode, const aiMaterial* aMaterial, std::unordered_map<std::string, std::string>& aInOutValues, std::unordered_map<std::string, tools::V3f>& aInOutColors)
	{
		const aiNode* node = aNode;

		std::stack<const aiNode*> stack;
		while (node)
		{
			stack.push(node);
			node = node->mParent;
		}
		while (!stack.empty())
		{
			node = stack.top();
			stack.pop();
			if (node->mMetaData)
			{
				aiMetadata* data = node->mMetaData;
				for (size_t i = 0; i < data->mNumProperties; i++)
				{
					if (data->mValues[i].mType == aiMetadataType::AI_AISTRING && strlen(((aiString*)data->mValues[i].mData)->C_Str()) > 1)
					{
						aInOutValues[data->mKeys[i].C_Str()] = ((aiString*)data->mValues[i].mData)->C_Str();
					}
				}
			}
		}

		aiString path;
		if (aMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == aiReturn::aiReturn_SUCCESS)
		{
			aInOutValues["Diffuse"] = path.C_Str();
		}
		if (aMaterial->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == aiReturn::aiReturn_SUCCESS)
		{
			aInOutValues["Normal"] = path.C_Str();
		}
		if (aMaterial->Get(AI_MATKEY_TEXTURE_REFLECTION(0), path) == aiReturn::aiReturn_SUCCESS)
		{
			aInOutValues["Reflection"] = path.C_Str();
		}
		if (aMaterial->Get(AI_MATKEY_TEXTURE_SHININESS(0), path) == aiReturn::aiReturn_SUCCESS)
		{
			aInOutValues["Shininess"] = path.C_Str();
		}
		if (aMaterial->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == aiReturn::aiReturn_SUCCESS)
		{
			aInOutValues["Specular"] = path.C_Str();
		}

		aiColor3D color;
		if (aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn::aiReturn_SUCCESS)
		{
			aInOutColors["Diffuse"] = tools::V3f(color.r, color.g, color.b);
		}
	}

	void ModelLoader::LoadModel_Internal(Model* aModel, const std::string& aFilePath)
	{
		static_assert(CHAR_BIT == 8, "Byte is not expected size");
		static_assert(sizeof(float) * CHAR_BIT == 32, "float is not 32bit");
		static_assert(sizeof(UINT) * CHAR_BIT == 32, "uint is not 32 bit");

		LOG_SYS_INFO("Loading model: " + aFilePath);


#pragma region Importing

		if (!tools::FileExists(aFilePath))
		{
			LOG_ERROR("File not found", aFilePath);
			return;
		}
		const aiScene* scene = aiImportFile(aFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

		if (!scene)
		{
			LOG_ERROR(aiGetErrorString(), aFilePath);
			return;
		}

		EXECUTE_ON_DESTRUCT({ aiReleaseImport(scene); });

		if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
		{
			LOG_ERROR("FBX is corrupt (Scene incomplete)", aFilePath);
			return;
		}
		if (!scene->HasMeshes())
		{
			LOG_ERROR("FBX has no meshes", aFilePath);
			return;
		}
#pragma endregion

		std::unordered_map<std::string, std::string> attributes;
		attributes["Animations"] = "animations.json";

		LoadNode(scene, scene->mRootNode, aiMatrix4x4(), aModel, attributes, aFilePath);

		bool hasAnimation = false;
		for (Model::ModelData* modelData : aModel->GetModelData())
		{
			if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
			{
				hasAnimation = true;
				break;
			}
		}
		//if (hasAnimation)
		//{
		//	aModel->myAnimations = AssetManager::GetInstance().GetJSONRelative(aFilePath, attributes["Animations"]);
		//}
		aModel->MarkLoaded();
	}


	void ModelLoader::LoadNode(const aiScene* aScene, const aiNode* aNode, aiMatrix4x4 aTransform, Model* aModel, std::unordered_map<std::string, std::string>& aInOutAttributes, const std::string& aFilePath)
	{
		for (size_t i = 0; i < aNode->mNumChildren; i++)
		{
			LoadNode(aScene, aNode->mChildren[i], aTransform, aModel, aInOutAttributes, aFilePath);
		}

		for (size_t i = 0; i < aNode->mNumMeshes; i++)
		{
			LoadMesh(aScene, aNode, aTransform * aNode->mTransformation, aScene->mMeshes[aNode->mMeshes[i]], aModel, aInOutAttributes, aFilePath);
		}
	}

	void ModelLoader::LoadMesh(const aiScene* aScene, const aiNode* aNode, aiMatrix4x4 aTransform, const aiMesh* aMesh, Model* aModel, std::unordered_map<std::string, std::string>& aInOutAttributes, const std::string& aFilePath)
	{
		ID3D11Device* device = graphics::GraphicsEngine::GetInstance().GetFrameWork().GetDevice();

		HRESULT result;

		ShaderFlags flags = ShaderTypes::FlagsFromMesh(aMesh);

		aInOutAttributes["VertexShader"] = "Model.hlsl";
		aInOutAttributes["PixelShader"] = myDefaultPixelShader;
		aInOutAttributes["Diffuse"] = "diffuse.dds";
		aInOutAttributes["Normal"] = "normal.dds";
		aInOutAttributes["Material"] = "material.dds";
		aInOutAttributes["Animations"] = "animations.json";

		std::unordered_map<std::string, tools::V3f> colorMappings;

		LoadAttributes(aNode, aScene->mMaterials[aMesh->mMaterialIndex], aInOutAttributes, colorMappings);

		AssetHandle vertexShader = AssetManager::GetInstance().GetVertexShader(aInOutAttributes["VertexShader"], flags);
		AssetHandle pixelShader = AssetManager::GetInstance().GetPixelShader(aInOutAttributes["PixelShader"], flags);

		if (!vertexShader.IsValid() || !pixelShader.IsValid())
		{
			return;
		}

		size_t vertexCount = 0;
		char* verticies = nullptr;
		EXECUTE_ON_DESTRUCT({ free(verticies); });

		LoadVerticies(aMesh, &verticies, &vertexCount, flags, aModel->myBoneNameLookup, aModel->myBoneData);


		size_t indexCount = aMesh->mNumFaces * 3;

		UINT* indexes = reinterpret_cast<UINT*>(malloc(sizeof(UINT) * indexCount));
		assert(indexes);
		tools::ExecuteOnDestruct freeIndexes = tools::ExecuteOnDestruct([indexes]() { free(indexes); });

		size_t count = 0;
		for (unsigned int j = 0; j < aMesh->mNumFaces; j++)
		{
			if (aMesh->mFaces[j].mNumIndices != 3)
			{
				LOG_ERROR("Ngon detected", aFilePath);
				return;
			}

			memcpy(indexes + count, aMesh->mFaces[j].mIndices, 3 * sizeof(UINT));
			count += 3;
		}


		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;

		ShaderTypes::Offsets offsets = ShaderTypes::OffsetsFromFlags(flags);
		CD3D11_BUFFER_DESC vertexBufferDescription;
		WIPE(vertexBufferDescription);
		vertexBufferDescription.ByteWidth = static_cast<UINT>(offsets.size * vertexCount);
		vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubresourceData;
		WIPE(vertexSubresourceData);
		vertexSubresourceData.pSysMem = verticies;

		result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create vertex buffer", aFilePath);
			return;
		}

		tools::ExecuteOnDestruct releaseVertexBuffer = tools::ExecuteOnDestruct([vertexBuffer]() { vertexBuffer->Release(); });

		CD3D11_BUFFER_DESC indexBufferDescription;
		WIPE(indexBufferDescription);
		indexBufferDescription.ByteWidth = static_cast<UINT>(indexCount * sizeof(UINT));
		indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubresourceData;
		ZeroMemory(&indexSubresourceData, sizeof(indexSubresourceData));
		indexSubresourceData.pSysMem = indexes;

		result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create index buffer", aFilePath);
			return;
		}
		tools::ExecuteOnDestruct releaseIndexBuffer = tools::ExecuteOnDestruct([indexBuffer]() { indexBuffer->Release(); });


		D3D11_INPUT_ELEMENT_DESC layout[ShaderTypes::MaxInputElementSize];
		UINT layoutElements = ShaderTypes::InputLayoutFromFlags(layout, flags);

		ID3D11InputLayout* inputLayout;
		const VertexShaderAsset& vsAsset = vertexShader.Access();
		result = device->CreateInputLayout(layout, layoutElements, vsAsset.myBlob.data(), vsAsset.myBlob.size(), &inputLayout);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to create inputlayout", aFilePath);
			return;
		}
		tools::ExecuteOnDestruct releaseInputLayout = tools::ExecuteOnDestruct([inputLayout]() { inputLayout->Release(); });

		Model::ModelData* modelData = new Model::ModelData();
		modelData->myshaderTypeFlags = flags;
		modelData->myStride = static_cast<UINT>(offsets.size);
		modelData->myOffset =
		{
			aTransform.a1,aTransform.a2,aTransform.a3,aTransform.a4,
			aTransform.b1,aTransform.b2,aTransform.b3,aTransform.b4,
			aTransform.c1,aTransform.c2,aTransform.c3,aTransform.c4,
			aTransform.d1,aTransform.d2,aTransform.d3,aTransform.d4
		};
		modelData->myVertexShader = vertexShader;
		modelData->myPixelShader = pixelShader;
		modelData->myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		modelData->myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
		modelData->myTextures[0] = AssetManager::GetInstance().GetTextureRelative(aFilePath, aInOutAttributes["Diffuse"], true);
		modelData->myTextures[1] = AssetManager::GetInstance().GetTextureRelative(aFilePath, aInOutAttributes["Normal"], true);
		modelData->myTextures[2] = AssetManager::GetInstance().GetTextureRelative(aFilePath, aInOutAttributes["Reflection"], true);
		modelData->myUseForwardRenderer = (aInOutAttributes["PixelShader"] != myDefaultPixelShader);
		modelData->myNumberOfIndexes = static_cast<UINT>(indexCount);
		modelData->myDiffuseColor = colorMappings["Diffuse"].Extend(1);

		modelData->myVertexBuffer = vertexBuffer;
		modelData->myIndexBuffer = indexBuffer;
		modelData->myInputLayout = inputLayout;

		releaseVertexBuffer.Disable();
		releaseIndexBuffer.Disable();
		releaseInputLayout.Disable();

		aModel->AddModelPart(modelData);
	}

	void ModelLoader::QueueLoad(Model* aModel, std::string aFilePath)
	{
		PERFORMANCETAG("Model loading enqueuement");
		while (myIsRunning)
		{
			for (size_t i = 0; i < myHandoverSlots; i++)
			{
				if (myHandovers[i].myEmpty)
				{
					myHandovers[i].myModel = aModel;
					myHandovers[i].myFilePath = aFilePath;
					myHandovers[i].myEmpty = false;
					return;
				}
			}
			std::this_thread::yield();
		}
	}

	void ModelLoader::PrepareModel(Model* aModel, const std::string& aPath)
	{
		QueueLoad(aModel, aPath);
	}

	Asset* ModelLoader::LoadModel(const std::string& aFilePath)
	{
		PERFORMANCETAG("Model loading");
		Model* model = new Model();
		PrepareModel(model, aFilePath);
		return new ModelAsset(model, aFilePath);
	}
}