#include "pch.h"
#include "ModelLoader.h"
#include <Windows.h>
#include <d3d11.h>
#include <assert.h>
#include <fstream>
#include "Model.h"
#include "ModelInstance.h"
#include "DirectX11Framework.h"
#include "ShaderCompiler.h"
#include "Scene.h"
#include "Macros.h"
#include "NameThread.h"
#include "Skybox.h"
#include <Vector4.hpp>
#include <Vector3.hpp>
#include "TextureLoader.h"
#include <queue>
#include <stack>
#include <unordered_set>
#include "ShaderTypes.h"
#include "AnimationData.h"
#include "AssetImportHelpers.h"

#include "AssetManager.h"

#pragma warning(push)
#pragma warning(disable: 26812)
#include "assimp\cimport.h"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#pragma warning(pop)

struct Vertex
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
	float btx, bty, btz, btw;
	float u, v;
};


bool does_file_exist(std::string fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

bool LoadVerticies(aiMesh* fbxMesh, char** aVertexBuffer, size_t* aVertexCount, size_t aFlags, std::unordered_map<std::string, unsigned int>& aBoneIndexMap, std::vector<BoneInfo>& aBoneInfo)
{
	SYSVERBOSE("Loading model with flags: " + ShaderTypes::PostfixFromFlags(aFlags));
	if (!(fbxMesh->HasPositions() && fbxMesh->HasTextureCoords(0) && fbxMesh->HasNormals() && fbxMesh->HasTangentsAndBitangents()))
	{
		SYSERROR("Model does not have all required data", "");
		return false;
	}

	ShaderTypes::Offsets offsets = ShaderTypes::OffsetsFromFlags(aFlags);

	static const float one = 1.0f;
	static const float zero = 0.0f;

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
					BoneIndex = CAST(unsigned int, aBoneIndexMap.size());
					BoneInfo bi;
					aBoneInfo.push_back(bi);


					CommonUtilities::Matrix4x4<float> NodeTransformation = AiHelpers::ConvertToEngineMatrix44(fbxMesh->mBones[i]->mOffsetMatrix);

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
			memcpy(&(buffer[(offsets.size * i + offsets.position)]), &(fbxMesh->mVertices[i]), sizeof(float) * 3);
			memcpy(&(buffer[(offsets.size * i + offsets.position + 3 * sizeof(float))]), &one, sizeof(float) * 1);

			memcpy(&(buffer[(offsets.size * i + offsets.normal)]), &fbxMesh->mNormals[i], sizeof(float) * 3);
			memcpy(&(buffer[(offsets.size * i + offsets.normal + 3 * sizeof(float))]), &zero, sizeof(float) * 1);

			memcpy(&(buffer[(offsets.size * i + offsets.tangent)]), &fbxMesh->mTangents[i], sizeof(float) * 3);
			memcpy(&(buffer[(offsets.size * i + offsets.tangent + 3 * sizeof(float))]), &zero, sizeof(float) * 1);
			memcpy(&(buffer[(offsets.size * i + offsets.bitanget)]), &fbxMesh->mBitangents[i], sizeof(float) * 3);
			memcpy(&(buffer[(offsets.size * i + offsets.bitanget + 3 * sizeof(float))]), &zero, sizeof(float) * 1);

			memcpy(&(buffer[(offsets.size * i + offsets.uv)]), &fbxMesh->mTextureCoords[0][i], sizeof(float) * 2);
		}
		if (aFlags & ShaderFlags::HasVertexColors)
		{
			for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
			{
				memcpy(&(buffer[(offsets.size * i + offsets.vertexcolor)]), &fbxMesh->mColors[0][i], sizeof(float) * 4);
			}
		}
		if (aFlags & ShaderFlags::HasUvSets)
		{
			for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
			{
				memcpy(&(buffer[(offsets.size * i + offsets.uv1)]), &fbxMesh->mTextureCoords[1][i], sizeof(float) * 2);
				memcpy(&(buffer[(offsets.size * i + offsets.uv2)]), &fbxMesh->mTextureCoords[2][i], sizeof(float) * 2);
			}
		}
		if (aFlags & ShaderFlags::HasBones)
		{
			size_t numberofBones = ShaderTypes::BonePerVertexCountFromFlags(aFlags);
			for (unsigned int i = offset; i < fbxMesh->mNumVertices; i += NUMTHREADS)
			{
				memcpy(&(buffer[(offsets.size * i + offsets.bones)]), &collectedBoneData[i].IDs, sizeof(UINT) * numberofBones);
				memcpy(&(buffer[(offsets.size * i + offsets.boneweights)]), &collectedBoneData[i].Weights, sizeof(float) * numberofBones);
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



ModelLoader::ModelLoader(ID3D11Device* aDevice)
{
	myWorkHorse = std::thread(std::bind(&ModelLoader::LoadLoop, this));
	myDevice = aDevice;
}

ModelLoader::~ModelLoader()
{
	myIsRunning = false;
	myWorkHorse.join();
}

struct LodToLoad
{
	size_t myLevel;
	aiNode* myNode;
	Model* myModel;
	const aiScene* myScene;
};

class CompareLevelInverse
{
public:
	bool operator()(const LodToLoad& a, const LodToLoad& b)
	{
		return a.myLevel > b.myLevel;
	}
};
class CompareVertexCountInverse
{
public:
	bool operator()(const LodToLoad& a, const LodToLoad& b)
	{
		SYSERROR("This should not be called until the FPS", "");
		return false;
	}
};

void ModelLoader::LoadLoop()
{
	NAMETHREAD(L"ModelLoader-Workhorse");
	LoadPackage package;


	std::priority_queue<LodToLoad, std::vector<LodToLoad>, CompareLevelInverse> lodQueue;
	std::unordered_set<const aiScene*> loadedScenes;

	auto LoadQueuedLods = [&]()
	{
		struct Index
		{
			UINT index;
		} *indexes = nullptr;
		while (!lodQueue.empty())
		{
			LodToLoad current = lodQueue.top();
			lodQueue.pop();

			ID3D11Buffer* vertexBuffer;
			ID3D11Buffer* indexBuffer;
			UINT indexCount = 0;
			size_t vertexCount = 0;

			char* verticies = nullptr;

			std::queue<const aiNode*> queue;
			queue.push(current.myNode);
			while (!queue.empty())
			{
				const aiNode* currentNode = queue.front();
				queue.pop();
				for (size_t i = 0; i < currentNode->mNumChildren; i++)
				{
					queue.push(currentNode->mChildren[i]);
				}

				for (size_t i = 0; i < currentNode->mNumMeshes; i++)
				{
					aiMesh* mesh = current.myScene->mMeshes[currentNode->mMeshes[i]];
					LoadVerticies(mesh, &verticies, &vertexCount, current.myModel->GetModelData()->myshaderTypeFlags, current.myModel->myBoneNameLookup, current.myModel->myBoneData);

					size_t count = indexCount;
					for (size_t j = 0; j < mesh->mNumFaces; j++)
					{
						indexCount += mesh->mFaces[j].mNumIndices;
					}

					Index* p = reinterpret_cast<Index*>(realloc(indexes, sizeof(Index) * indexCount));

					if (!p)
					{
						free(indexes);
					}

					indexes = p;

					for (unsigned int j = 0; j < mesh->mNumFaces; j++)
					{
						memcpy(indexes + count, mesh->mFaces[j].mIndices, mesh->mFaces[j].mNumIndices * sizeof(Index));
						count += mesh->mFaces[j].mNumIndices;
					}
				}
			}



#if DEBUGBONES
			std::unordered_map <std::string, aiNode*> nodeNameLookup;
			std::stack<aiNode*> myQueue;
			myQueue.push(current.myScene->mRootNode);
			while (!myQueue.empty())
			{
				aiNode* node = myQueue.top();
				myQueue.pop();
				for (size_t i = 0; i < node->mNumChildren; i++)
				{
					myQueue.push(node->mChildren[i]);
				}
				nodeNameLookup[node->mName.C_Str()] = node;
			}

			V4F rootbone;
			for (auto& i : current.myModel->myBoneData)
			{
				if (nodeNameLookup.count(i.myName) != 0)
				{
					aiNode* parent = nodeNameLookup[i.myName]->mParent;

					V4F pos{ 0,0,0,1 };
					while (parent)
					{
						if (i.parent == -1 && current.myModel->myBoneNameLookup.count(parent->mName.C_Str()) != 0)
						{
							i.parent = current.myModel->myBoneNameLookup[parent->mName.C_Str()];
						}
						pos = pos * M44F::GetFastInverse(M44F::Transpose(AiHelpers::ConvertToEngineMatrix44(parent->mTransformation)));
						//pos = pos * M44F::GetFastInverse(M44F::Transpose(AiHelpers::ConvertToEngineMatrix44(nodeNameLookup[i.myName]->mTransformation)));

						parent = parent->mParent;
					}
					if (i.parent == -1)
					{
						rootbone = pos;
					}
					i.BonePosition = pos;
				}
			}
			for (auto& i : current.myModel->myBoneData)
			{
				i.BonePosition -= rootbone;
				i.BonePosition.y = -i.BonePosition.y;
				i.BonePosition.RotateY(-PI / 2.f);
				i.BonePosition.RotateZ(PI / 2.f);
				i.BonePosition -= rootbone;
			}


#endif // DEBUGBONES


#ifndef _RETAIL
			unsigned int LodFaceLimits[] =
			{
				20000,
				15000,
				12500,
				10000,
				7000,
				5000,
				3000,
				2000
			};

			unsigned int HardLodFaceLimits[] =
			{
				40000,
				30000,
				25000,
				20000,
				14000,
				10000,
				6000,
				4000
			};
			size_t faceCount = indexCount / 3;
			if (faceCount > HardLodFaceLimits[current.myLevel])
			{
				SYSERROR("Model has way too many faces in lod level " + std::to_string(current.myLevel) + " (" + std::to_string(faceCount) + " faceslimit: " + std::to_string(LodFaceLimits[current.myLevel]) + ")", current.myModel->GetFriendlyName());
				SYSERROR("Wont load model due to way to many faces", package.myFilePath);
				continue;
			}

			if (faceCount > LodFaceLimits[current.myLevel])
			{
				SYSWARNING("Model has too many faces in lod level " + std::to_string(current.myLevel) + " (" + std::to_string(faceCount) + " faces limit: " + std::to_string(LodFaceLimits[current.myLevel]) + ")", current.myModel->GetFriendlyName());
			}
#endif // !_RETAIL
			ShaderTypes::Offsets offsets = ShaderTypes::OffsetsFromFlags(current.myModel->GetModelData()->myshaderTypeFlags);
			CD3D11_BUFFER_DESC vertexBufferDescription;
			WIPE(vertexBufferDescription);
			vertexBufferDescription.ByteWidth = CAST(UINT, offsets.size * vertexCount);
			vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertexSubresourceData;
			WIPE(vertexSubresourceData);
			vertexSubresourceData.pSysMem = verticies;

			DirectX11Framework::AddMemoryUsage(vertexBufferDescription.ByteWidth,std::filesystem::path(current.myModel->GetModelData()->myFilePath).filename().string(),"Model Vertex Buffer");

			HRESULT result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			if (FAILED(result))
			{
				SYSERROR("Couldn not create vertex buffer for model ", package.myFilePath);
				free(verticies);
				continue;
			}

#ifdef _DEBUG
			if (indexCount % 3 != 0)
			{
				SYSERROR("Indexcount for model is not a multiple of three", package.myFilePath);
				free(verticies);
				continue;
			}
#endif // _DEBUG





			CD3D11_BUFFER_DESC indexBufferDescription;
			WIPE(indexBufferDescription);
			indexBufferDescription.ByteWidth = indexCount * sizeof(Index);
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA indexSubresourceData;
			ZeroMemory(&indexSubresourceData, sizeof(indexSubresourceData));
			indexSubresourceData.pSysMem = indexes;

			DirectX11Framework::AddMemoryUsage(indexBufferDescription.ByteWidth, std::filesystem::path(current.myModel->GetModelData()->myFilePath).filename().string(), "Model Index Buffer");


			result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			if (FAILED(result))
			{
				SYSERROR("Could not create index buffer for", package.myFilePath);
				free(verticies);
				free(indexes);
				continue;
			}
			float graphicSize = 0;
			for (size_t i = 0; i < vertexCount; i++)
			{
				graphicSize = MAX(graphicSize, reinterpret_cast<V3F*>(&verticies[i * offsets.size + offsets.position])->LengthSqr());
			}
			delete[] verticies;

			Model::LodLevel* level = new Model::LodLevel;
#ifndef _RETAIL
			if (level)
			{
#endif // !_RETAIL
				level->myIndexBuffer = indexBuffer;
				level->myVertexBuffer = new ID3D11Buffer*(vertexBuffer);
				level->myNumberOfIndexes = indexCount;
				current.myModel->ApplyLodLevel(level, current.myLevel, sqrt(graphicSize));
#ifndef _RETAIL
			}
			else
			{
				SYSERROR("Skipping model loading due to lack of memory", "");
				continue;
			}
#endif // !_RETAIL

		}
		for (auto& i : loadedScenes)
		{
			aiReleaseImport(i);
		}
		loadedScenes.clear();
		free(indexes);
	};

	while (myIsRunning)
	{
		package.myEmpty = true;
		for (size_t i = 0; i < myHandoverSlots; i++)
		{
			if (!myHandovers[i].myEmpty && myHandovers[i].myModel->ShouldLoad())
			{
				package = myHandovers[i];
				myHandovers[i].myEmpty = true;
				break;
			}
		}
		if (package.myEmpty)
		{
			LoadQueuedLods();
			std::this_thread::yield();
			continue;
		}



		HRESULT result;

#pragma region BufferSetup


		static_assert(CHAR_BIT == 8, "i mean what!?");
		static_assert(sizeof(float) * CHAR_BIT == 32, "Things are fucked beyond comprehension");
		static_assert(sizeof(UINT) * CHAR_BIT == 32, "you get the point");

		const aiScene* scene = NULL;

		if (!does_file_exist(package.myFilePath))
		{
			SYSERROR("File not found", package.myFilePath);
			continue;
		}
		scene = aiImportFile(package.myFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

		SYSINFO("Loading model: " + package.myFilePath);

		if (!scene)
		{
			SYSERROR(aiGetErrorString(), package.myFilePath);
			continue;
		}
		if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
		{
			SYSERROR("FBX is corrupt (Scene incomplete)", package.myFilePath);
			aiReleaseImport(scene);
			continue;
		}
		if (!scene->HasMeshes())
		{
			SYSERROR("FBX is corrupt (Has no meshes)", package.myFilePath);
			aiReleaseImport(scene);
			continue;
		}
		ShaderFlags flags = ShaderTypes::FlagsFromMesh(scene->mMeshes[0]);
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			ShaderFlags flags2 = ShaderTypes::FlagsFromMesh(scene->mMeshes[i]);
			if (flags != flags2)
			{
				SYSERROR("FBX is corrupt (inconsistent vertex types) [" + ShaderTypes::PostfixFromFlags(flags) + "] != [" + ShaderTypes::PostfixFromFlags(flags2) + "]", package.myFilePath);
				aiReleaseImport(scene);
				continue;
			}
		}


		loadedScenes.insert(scene);

#pragma endregion

		//////////////////////////////////////////////

#pragma region Shaders

		std::string vertexShaderPath = "Model.hlsl";
		std::string pixelShaderPath = "fullscreen_deferred/ToonShader.hlsl";
		std::string AlbedoPath = "material.dds";
		std::string NormalPath = "normal.dds";
		std::string MaterialPath = "material.dds";
		std::string AnimationPath = "animations.json";
		std::unordered_map<std::string, std::string*> attributeMapping;
		attributeMapping["VertexShader"] = &vertexShaderPath;
		attributeMapping["PixelShader"] = &pixelShaderPath;

		bool forceForward = false;

		if (scene->mNumMaterials == 0)
		{
			SYSWARNING("Model has no materials skipping", package.myFilePath);
			continue;
		}
		if (scene->mNumMaterials > 1)
		{
			SYSWARNING("Model has more than 1 material only first will be used", package.myFilePath);
		}

		aiMaterial* mat = scene->mMaterials[0];

		aiString path;
		if (mat->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == aiReturn::aiReturn_SUCCESS)
		{
			AlbedoPath = path.C_Str();
		}
		if (mat->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == aiReturn::aiReturn_SUCCESS)
		{
			NormalPath = path.C_Str();
		}


		SYSVERBOSE("Mapping attributes for: " + package.myFilePath);
		std::queue<aiNode*> queue;
		queue.push(scene->mRootNode);
		while (!queue.empty())
		{
			aiNode* node = queue.front();
			queue.pop();
			for (size_t i = 0; i < node->mNumChildren; i++)
			{
				queue.push(node->mChildren[i]);
			}
			if (node->mMetaData)
			{
				aiMetadata* data = node->mMetaData;
				for (size_t i = 0; i < data->mNumProperties; i++)
				{
					if (data->mValues[i].mType == aiMetadataType::AI_AISTRING && strlen(((aiString*)data->mValues[i].mData)->C_Str()) > 1)
					{
						auto it = attributeMapping.find(data->mKeys[i].C_Str());
						if (it != attributeMapping.end())
						{
							*(it->second) = ((aiString*)data->mValues[i].mData)->C_Str();
						}
					}
				}
			}
		}

		if (pixelShaderPath != "fullscreen_deferred/ToonShader.hlsl")
		{
			forceForward = true;
		}

		AssetHandle vertexShader = AssetManager::GetInstance().GetVertexShader(vertexShaderPath, flags);
		AssetHandle pixelShader = AssetManager::GetInstance().GetPixelShader(pixelShaderPath, flags);

		if (!vertexShader.IsValid() || !pixelShader.IsValid())
		{
			continue;
		}

#pragma endregion
#pragma region Layout

		size_t layoutElements;
		D3D11_INPUT_ELEMENT_DESC* layout = ShaderTypes::InputLayoutFromFlags(flags, layoutElements);

		ID3D11InputLayout* inputLayout;
		result = myDevice->CreateInputLayout(layout, CAST(UINT, layoutElements), vertexShader.GetVertexShaderblob().data(), vertexShader.GetVertexShaderblob().size(), &inputLayout);
		if (FAILED(result))
		{
			SYSERROR("Could not create inputlayout for: ", package.myFilePath);
			continue;
		}
#pragma endregion

		//////////////////////////////////////////////

#pragma region Model

		Model::ModelData modelData;
		modelData.myshaderTypeFlags = flags;
		modelData.myStride = CAST(UINT, ShaderTypes::OffsetsFromFlags(flags).size);
		modelData.myOffset = 0;
		modelData.myVertexShader = vertexShader;
		modelData.myPixelShader = pixelShader;
		modelData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		modelData.myInputLayout = inputLayout;
		modelData.myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
		modelData.myTextures[0] = AssetManager::GetInstance().GetTextureRelative(package.myFilePath, AlbedoPath, true);
		modelData.myTextures[1] = AssetManager::GetInstance().GetTextureRelative(package.myFilePath, NormalPath, true);
		modelData.myTextures[2] = AssetManager::GetInstance().GetTextureRelative(package.myFilePath, MaterialPath, true);
		modelData.myFilePath = package.myFilePath;
		modelData.myForceForward = forceForward;

		if (flags & ShaderFlags::HasBones)
		{
			modelData.myAnimations = AssetManager::GetInstance().GetJSONRelative(package.myFilePath, AnimationPath);
		}


		package.myModel->Init(modelData, this, pixelShaderPath, vertexShaderPath, package.myFilePath, package.myFilePath);

#pragma endregion

		int lodCount = 0;

		for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++)
		{
			if (std::string(scene->mRootNode->mChildren[i]->mName.C_Str()).substr(0, 4) == std::string("lod1"))
			{
				LodToLoad lod;
				lod.myNode = scene->mRootNode->mChildren[i];
				lod.myModel = package.myModel;
				lod.myScene = scene;
				lod.myLevel = 0;
				lodQueue.push(lod);
				++lodCount;
			}
			else if (std::string(scene->mRootNode->mChildren[i]->mName.C_Str()).substr(0, 4) == std::string("lod2"))
			{
				LodToLoad lod;
				lod.myNode = scene->mRootNode->mChildren[i];
				lod.myModel = package.myModel;
				lod.myScene = scene;
				lod.myLevel = 1;
				lodQueue.push(lod);
				++lodCount;
			}
			else if (std::string(scene->mRootNode->mChildren[i]->mName.C_Str()).substr(0, 4) == std::string("lod3"))
			{
				LodToLoad lod;
				lod.myNode = scene->mRootNode->mChildren[i];
				lod.myModel = package.myModel;
				lod.myScene = scene;
				lod.myLevel = 2;
				lodQueue.push(lod);
				++lodCount;
			}
			else
			{
				SYSWARNING("lod name not recognized", scene->mRootNode->mChildren[i]->mName.C_Str());
			}
		}
		if (lodCount == 0)
		{
			SYSERROR("Model has no LOD Level(s)", package.myFilePath);


			LodToLoad lod;
			lod.myNode = scene->mRootNode;
			lod.myModel = package.myModel;
			lod.myScene = scene;
			lod.myLevel = 0;
			lodQueue.push(lod);
			++lodCount;

			package.myModel->myIsMissNamed = true;
		}
		else if (lodCount < 2)
		{
			SYSWARNING("Model only has " + std::to_string(lodCount) + " LOD Level(s)", package.myFilePath);
		}


	}

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
	return new ModelAsset(model);
}


Asset* ModelLoader::LoadSkybox(const std::string& aFilePath)
{
	static size_t skyBoxCounter = 0;
	++skyBoxCounter;


	HRESULT result;

#pragma region BufferSetup

	static_assert(sizeof(float) == 4, "Things are fucked beyond comprehension");

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT indexCount;

	{

		float sideLength = 2.6f;
		Vertex verticies[] =
		{
			{ sideLength, sideLength, sideLength ,1,	1,0,0,1		,1.f,0.f}, // top left
			{ sideLength, sideLength,-sideLength ,1,	0,1,0.4f,1	,1.f,0.f}, // top right
			{ sideLength,-sideLength, sideLength ,1,	0,1,0.4f,1	,1.f,1.f}, // bottom left
			{-sideLength, sideLength, sideLength ,1,	0.4f,1,0,1	,0.f,0.f}, // bottom right
			{ sideLength,-sideLength,-sideLength ,1,	0.4f,1,0,1	,1.f,1.f}, // ^^ but back
			{-sideLength, sideLength,-sideLength ,1,	0,1,0.4f,1	,0.f,0.f},
			{-sideLength,-sideLength, sideLength ,1,	0,1,0.4f,1	,0.f,1.f},
			{-sideLength,-sideLength,-sideLength ,1,	0,0,1,1		,0.f,1.f},
		};

		CD3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));
		vertexBufferDescription.ByteWidth = sizeof(verticies);
		vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubresourceData;
		ZeroMemory(&vertexSubresourceData, sizeof(vertexSubresourceData));
		vertexSubresourceData.pSysMem = verticies;

		DirectX11Framework::AddMemoryUsage(vertexBufferDescription.ByteWidth, "SkyBox", "Model Vertex Buffer");
		result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
		if (FAILED(result))
		{
			SYSERROR("could not create vertex buffer for Skybox", "");
			return nullptr;
		}

		UINT indexes[] =
		{
			2,0,  1,
			1,0,  3,
			3,0,  2,
			2,1,  4,
			3,2,  6,
			1,3,  5,
			5,4,  1,
			6,5,  3,
			4,6,  2,
			6,7,  5,
			5,7,  4,
			4,7,  6
		};

		assert(sizeof(indexes) / sizeof(indexes[0]) % 3 == 0 && "Not Multiple of 3");

		CD3D11_BUFFER_DESC indexBufferDescription;
		ZeroMemory(&indexBufferDescription, sizeof(indexBufferDescription));
		indexBufferDescription.ByteWidth = sizeof(indexes);
		indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubresourceData;
		ZeroMemory(&indexSubresourceData, sizeof(indexSubresourceData));
		indexSubresourceData.pSysMem = indexes;

		DirectX11Framework::AddMemoryUsage(indexBufferDescription.ByteWidth, "SkyBox", "Model Index Buffer");
		result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
		if (FAILED(result))
		{
			SYSERROR("Could not create index buffer for Skybox", "");
			return nullptr;
		}

		indexCount = sizeof(indexes) / sizeof(indexes[0]);
	}


#pragma endregion

	//////////////////////////////////////////////

#pragma region Shaders

	AssetHandle vertexShader = AssetManager::GetInstance().GetVertexShader("Skybox.hlsl");
	AssetHandle pixelShader = AssetManager::GetInstance().GetPixelShader("Skybox.hlsl");

#pragma endregion

	//////////////////////////////////////////////

#pragma region Layout

	size_t layoutelements;
	D3D11_INPUT_ELEMENT_DESC* layout = ShaderTypes::InputLayoutFromFlags(0, layoutelements);

	ID3D11InputLayout* inputLayout;
	result = myDevice->CreateInputLayout(layout, CAST(UINT, layoutelements), vertexShader.GetVertexShaderblob().data(), vertexShader.GetVertexShaderblob().size(), &inputLayout);
	if (FAILED(result))
	{
		SYSERROR("could not create input layout", aFilePath);
		return nullptr;
	}
#pragma endregion

	//////////////////////////////////////////////

#pragma region Model
	Model* model = new Model();
	if (!model)
	{
		return nullptr;
	}

	Model::LodLevel* level = new Model::LodLevel();
	level->myNumberOfIndexes = indexCount;
	level->myVertexBuffer = new ID3D11Buffer * (vertexBuffer);
	level->myIndexBuffer = indexBuffer;


	Model::ModelData modelData;
	modelData.myStride = sizeof(Vertex);
	modelData.myOffset = 0;
	modelData.myVertexShader = vertexShader;
	modelData.myPixelShader = pixelShader;
	modelData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelData.myInputLayout = inputLayout;
	modelData.myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
	modelData.myTextures[0] = AssetManager::GetInstance().GetCubeTexture(aFilePath);
	modelData.myTextures[1] = nullptr;
	modelData.myTextures[2] = nullptr;


	model->Init(modelData, this, "Skybox.hlsl", "Skybox.hlsl", "", "SkyBox: " + std::string(aFilePath.begin(), aFilePath.end()));
	model->ApplyLodLevel(level, 0);

	return new ModelAsset(model);
}

