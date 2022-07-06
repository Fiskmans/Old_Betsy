#if 0
#include "ModelLoader.h"
#include <Windows.h>
#include <d3d11.h>
#include <assert.h>
#include <fstream>
#include "Model.h"
#include "ModelInstance.h"
#include "DirectX11Framework.h"
#include "ShaderCompiler.h"
#include "RenderScene.h"
#include "Macros.h"
#include "NameThread.h"
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

bool LoadVerticies(const aiMesh* fbxMesh, char** aVertexBuffer, size_t* aVertexCount, size_t aFlags, std::unordered_map<std::string, unsigned int>& aBoneIndexMap, std::vector<BoneInfo>& aBoneInfo)
{
	SYSVERBOSE("Loading model with flags: " + ShaderTypes::PostfixFromFlags(aFlags));
	if (!fbxMesh->HasPositions())
	{
		SYSERROR("Model does not have positions");
		return false;
	}
	if(!fbxMesh->HasNormals())
	{
		SYSERROR("Model does not have normals");
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
				V4F* normal = reinterpret_cast<V4F*>(buffer + (offsets.size * i + offsets.normal));
				V4F* tangent = reinterpret_cast<V4F*>(buffer + (offsets.size * i + offsets.tangent));
				V4F* biTangent = reinterpret_cast<V4F*>(buffer + (offsets.size * i + offsets.bitanget));
				if ((*normal) == V4F(0,1,0,1))
				{
					*tangent = V4F(1, 0, 0, 1);
					*biTangent = V4F(0, 0, 1, 1);
				}
				else
				{
					*tangent = V4F(V3F(*normal).Cross({ 0, 1, 0 }).GetNormalized(), 1);
					*biTangent = V4F(V3F(*normal).Cross(V3F(*tangent)).GetNormalized(), 1);
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

ModelLoader::ModelLoader(ID3D11Device* aDevice, const std::string& aDefaultPixelShader)
{
	myDefaultPixelShader = aDefaultPixelShader;
	myDevice = aDevice;
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
		LoadPackage package;
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

void ModelLoader::LoadAttributes(const aiNode* aNode, const aiMaterial* aMaterial, std::unordered_map<std::string, std::string>& aInOutValues, std::unordered_map<std::string, V3F>& aInOutColors)
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
		aInOutValues["Shinyness"] = path.C_Str();
	}
	if (aMaterial->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == aiReturn::aiReturn_SUCCESS)
	{
		aInOutValues["Specular"] = path.C_Str();
	}

	aiColor3D color;
	if (aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn::aiReturn_SUCCESS)
	{
		aInOutColors["Diffuse"] = V3F(color.r, color.g, color.b);
	}
}

void ModelLoader::LoadModel_Internal(Model* aModel, const std::string& aFilePath)
{
	static_assert(CHAR_BIT == 8, "Byte is not expected size");
	static_assert(sizeof(float) * CHAR_BIT == 32, "float is not 32bit");
	static_assert(sizeof(UINT) * CHAR_BIT == 32, "uint is not 32 bit");

	SYSINFO("Loading model: " + aFilePath);


#pragma region Importing

	if (!Tools::FileExists(aFilePath))
	{
		SYSERROR("File not found", aFilePath);
		return;
	}
	const aiScene* scene = aiImportFile(aFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

	if (!scene)
	{
		SYSERROR(aiGetErrorString(), aFilePath);
		return;
	}

	Tools::ExecuteOnDestruct releaseScene = Tools::ExecuteOnDestruct([scene]() -> void {aiReleaseImport(scene); });

	if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
	{
		SYSERROR("FBX is corrupt (Scene incomplete)", aFilePath);
		return;
	}
	if (!scene->HasMeshes())
	{
		SYSERROR("FBX has no meshes", aFilePath);
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
	if (hasAnimation)
	{
		aModel->myAnimations = AssetManager::GetInstance().GetJSONRelative(aFilePath, attributes["Animations"]);
	}
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
	HRESULT result;

	ShaderFlags flags = ShaderTypes::FlagsFromMesh(aMesh);

	aInOutAttributes["VertexShader"] = "Model.hlsl";
	aInOutAttributes["PixelShader"] = myDefaultPixelShader;
	aInOutAttributes["Diffuse"] = "diffuse.dds";
	aInOutAttributes["Normal"] = "normal.dds";
	aInOutAttributes["Material"] = "material.dds";
	aInOutAttributes["Animations"] = "animations.json";

	std::unordered_map<std::string, V3F> colorMappings;

	LoadAttributes(aNode, aScene->mMaterials[aMesh->mMaterialIndex], aInOutAttributes, colorMappings);

	AssetHandle vertexShader = AssetManager::GetInstance().GetVertexShader(aInOutAttributes["VertexShader"], flags);
	AssetHandle pixelShader = AssetManager::GetInstance().GetPixelShader(aInOutAttributes["PixelShader"], flags);

	if (!vertexShader.IsValid() || !pixelShader.IsValid())
	{
		return;
	}

	size_t vertexCount = 0;
	char* verticies = nullptr;
	Tools::ExecuteOnDestruct freeVerticies = Tools::ExecuteOnDestruct([verticies]() { free(verticies); });

	LoadVerticies(aMesh, &verticies, &vertexCount, flags, aModel->myBoneNameLookup, aModel->myBoneData);


	size_t indexCount = aMesh->mNumFaces * 3;

	UINT* indexes = reinterpret_cast<UINT*>(malloc(sizeof(UINT) * indexCount));
	assert(indexes);
	Tools::ExecuteOnDestruct freeIndexes = Tools::ExecuteOnDestruct([indexes]() { free(indexes); });

	size_t count = 0;
	for (unsigned int j = 0; j < aMesh->mNumFaces; j++)
	{
		if (aMesh->mFaces[j].mNumIndices != 3)
		{
			SYSERROR("Ngon detected", aFilePath);
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

	result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
	if (FAILED(result))
	{
		SYSERROR("Couldn not create vertex buffer", aFilePath);
		return;
	}

	Tools::ExecuteOnDestruct releaseVertexBuffer = Tools::ExecuteOnDestruct([vertexBuffer]() { vertexBuffer->Release(); });

	CD3D11_BUFFER_DESC indexBufferDescription;
	WIPE(indexBufferDescription);
	indexBufferDescription.ByteWidth = static_cast<UINT>(indexCount * sizeof(UINT));
	indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData;
	ZeroMemory(&indexSubresourceData, sizeof(indexSubresourceData));
	indexSubresourceData.pSysMem = indexes;

	result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
	if (FAILED(result))
	{
		SYSERROR("Could not create index buffer", aFilePath);
		return;
	}
	Tools::ExecuteOnDestruct releaseIndexBuffer = Tools::ExecuteOnDestruct([indexBuffer]() { indexBuffer->Release(); });


	D3D11_INPUT_ELEMENT_DESC layout[ShaderTypes::MaxInputElementSize];
	UINT layoutElements = ShaderTypes::InputLayoutFromFlags(layout, flags);

	ID3D11InputLayout* inputLayout;
	result = myDevice->CreateInputLayout(layout, layoutElements, vertexShader.GetVertexShaderblob().data(), vertexShader.GetVertexShaderblob().size(), &inputLayout);
	if (FAILED(result))
	{
		SYSERROR("Could not create inputlayout", aFilePath);
		return;
	}
	Tools::ExecuteOnDestruct releaseInputLayout= Tools::ExecuteOnDestruct([inputLayout]() { inputLayout->Release(); });

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
	modelData->myDiffuseColor = colorMappings["Diffuse"];

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

		GraphicsFramework::AddGraphicsMemoryUsage(vertexBufferDescription.ByteWidth, "SkyBox", "Model Vertex Buffer");
		result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
		if (FAILED(result))
		{
			SYSERROR("could not create vertex buffer for Skybox");
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

		GraphicsFramework::AddGraphicsMemoryUsage(indexBufferDescription.ByteWidth, "SkyBox", "Model Index Buffer");
		result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
		if (FAILED(result))
		{
			SYSERROR("Could not create index buffer for Skybox");
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

	D3D11_INPUT_ELEMENT_DESC layout[ShaderTypes::MaxInputElementSize];
	UINT layoutelements = ShaderTypes::InputLayoutFromFlags(layout, ShaderFlags::HasUvSets);

	ID3D11InputLayout* inputLayout;
	result = myDevice->CreateInputLayout(layout, layoutelements, vertexShader.GetVertexShaderblob().data(), vertexShader.GetVertexShaderblob().size(), &inputLayout);
	if (FAILED(result))
	{
		SYSERROR("could not create input layout", aFilePath);
		return nullptr;
	}
#pragma endregion

	//////////////////////////////////////////////

#pragma region Model
	Model* model = new Model();

	Model::ModelData* modelData = new Model::ModelData();
	modelData->myStride = sizeof(Vertex);
	modelData->myOffset = M44f::Identity();
	modelData->myVertexShader = vertexShader;
	modelData->myPixelShader = pixelShader;
	modelData->myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelData->myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
	modelData->myTextures[0] = AssetManager::GetInstance().GetCubeTexture(aFilePath);
	modelData->myTextures[1] = nullptr;
	modelData->myTextures[2] = nullptr;
	modelData->myNumberOfIndexes = indexCount;

	modelData->myInputLayout = inputLayout;
	modelData->myVertexBuffer = vertexBuffer;
	modelData->myIndexBuffer = indexBuffer;


	model->AddModelPart(modelData);
	model->MarkLoaded();

	return new SkyboxAsset(model);
}

#endif