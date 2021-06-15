#include "pch.h"
#include "Terrain.h"

#include "GamlaBettan\DirectX11Framework.h"
#include "GamlaBettan\AssetManager.h"
#include "GamlaBettan\Model.h"
#include "GamlaBettan\Scene.h"

Terrain::Terrain()
	: myFramework(nullptr)
	, myModelAsset(nullptr)
	, myModelInstance(nullptr)
{
}

void Terrain::Init(DirectX11Framework* aFramework)
{
	myFramework = aFramework;
	TestMesh();
	Setup();
	SetupAllNormals();
}

void Terrain::Update()
{
	bool isDirty = true;


	if (isDirty)
	{
		if (!OverWriteBuffer(myModelData->myVertexBuffer,myVertexes.data(),myVertexes.size() * sizeof(Vertex)))
		{
			Scene::GetInstance().RemoveFromScene(myModelInstance);
			return;
		}
		if (!OverWriteBuffer(myModelData->myIndexBuffer, myTris.data(), myTris.size() * sizeof(Tri)))
		{
			Scene::GetInstance().RemoveFromScene(myModelInstance);
			return;
		}
		myModelData->myNumberOfIndexes = myTris.size() * 3;
	}
}

#if USEIMGUI
void Terrain::Imgui()
{
	WindowControl::Window("Terrain", [this]()
		{
			static bool showNormals;
			ImGui::Checkbox("Show Normals", &showNormals);

			if (showNormals)
			{
				DebugDrawer::GetInstance().SetColor(V4F(0,0.5,0,1));
				for (Vertex& vertex : myVertexes)
				{
					DebugDrawer::GetInstance().DrawDirection(vertex.myPosition, vertex.myNormal);
				}
			}
		});
}
#endif

void Terrain::Setup()
{
	Model* model = new Model();
	myModelData = new Model::ModelData();

	myModelData->myshaderTypeFlags = ShaderFlags::None;
	myModelData->myStride = sizeof(Vertex);
	myModelData->myOffset = M44f::Identity();

	myModelData->myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	myModelData->myIndexBufferFormat = DXGI_FORMAT_R32_UINT;

	myModelData->myVertexShader = AssetManager::GetInstance().GetVertexShader("engine/terrain.hlsl");
	myModelData->myPixelShader = AssetManager::GetInstance().GetPixelShader("engine/terrain.hlsl");

	SetupGraphicsResources();

	myModelData->myNumberOfIndexes = 0;

	myModelData->myUseForwardRenderer = false;

	model->AddModelPart(myModelData);
	myModelAsset = new ModelAsset(model);
	myModelHandle = AssetHandle(myModelAsset);

	model->MarkLoaded();

	myModelInstance = myModelHandle.InstansiateModel();

	Scene::GetInstance().AddToScene(myModelInstance);
}

void Terrain::SetupGraphicsResources()
{
	ID3D11Device* device = myFramework->GetDevice();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	CD3D11_BUFFER_DESC vertexBufferDescription;
	WIPE(vertexBufferDescription);
	vertexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(Vertex) * MaxTriCount * 3);
	vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	HRESULT result = device->CreateBuffer(&vertexBufferDescription, nullptr, &vertexBuffer);
	if (FAILED(result))
	{
		SYSERROR("Couldn not create vertex buffer", "Terrain");
		return;
	}

	Tools::ExecuteOnDestruct releaseVertexBuffer = Tools::ExecuteOnDestruct([vertexBuffer]() { vertexBuffer->Release(); });

	CD3D11_BUFFER_DESC indexBufferDescription;
	WIPE(indexBufferDescription);
	indexBufferDescription.ByteWidth = MaxTriCount * sizeof(Tri);
	indexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	result = device->CreateBuffer(&indexBufferDescription, nullptr, &indexBuffer);
	if (FAILED(result))
	{
		SYSERROR("Could not create index buffer", "Terrain");
		return;
	}
	Tools::ExecuteOnDestruct releaseIndexBuffer = Tools::ExecuteOnDestruct([indexBuffer]() { indexBuffer->Release(); });


	D3D11_INPUT_ELEMENT_DESC layout[2] =
	{
		{ "SV_POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",				0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ID3D11InputLayout* inputLayout;
	result = device->CreateInputLayout(layout, 2, myModelData->myVertexShader.GetVertexShaderblob().data(), myModelData->myVertexShader.GetVertexShaderblob().size(), &inputLayout);
	if (FAILED(result))
	{
		SYSERROR("Could not create inputlayout", "Terrain");
		return;
	}
	Tools::ExecuteOnDestruct releaseInputLayout = Tools::ExecuteOnDestruct([inputLayout]() { inputLayout->Release(); });


	myModelData->myVertexBuffer = vertexBuffer;
	myModelData->myIndexBuffer = indexBuffer;
	myModelData->myInputLayout = inputLayout;

	releaseVertexBuffer.Disable();
	releaseIndexBuffer.Disable();
	releaseInputLayout.Disable();
}

void Terrain::SetupAllNormals()
{
	std::vector<UINT> changed = {  };
	for (UINT i = 0; i < myVertexes.size(); i++)
	{
		changed.push_back(i);
	}
	SetupNormals(changed);
}

void Terrain::SetupNormals(UINT aVertexThatChanged)
{
	std::vector<UINT> changed = { aVertexThatChanged };
	for (auto& edge : myVertexInfo[aVertexThatChanged].myEdges)
	{
		changed.push_back(edge.myOtherVertex);
	}
	SetupNormals(changed);
}

void Terrain::SetupNormals(std::vector<UINT> aVertexesAffected)
{
	for (UINT& index : aVertexesAffected)
	{
		V3F totalNormal = V3F(0,0,0);
		for (VertexInfo::Edge& edge : myVertexInfo[index].myEdges)
		{
			Tri& tri = myTris[edge.myTri];
			totalNormal +=
				(myVertexes[tri.myA].myPosition - myVertexes[tri.myB].myPosition).Cross(
					(myVertexes[tri.myA].myPosition - myVertexes[tri.myC].myPosition)
				).GetNormalized();
		}
		myVertexes[index].myNormal = totalNormal.GetNormalized();
	}
}

void Terrain::TestMesh()
{
	const float size = 5_m;
	myVertexes = 
	{
		{ V3F(-size / 2,	 0,		 size / 2),	V3F(-1,  0,  0) }, // left
		{ V3F( size / 2,	 0,		 size / 2),	V3F( 1,  0,  0) }, // right
		{ V3F(0,			 0,		-size / 2), V3F( 0,  0, -1) }, // front
		{ V3F(0,			-size,   0),		V3F( 0, -1,  0) }  // down
	};

	myTris =
	{
		{ 0, 1, 2 },
		{ 1, 0, 3 },
		{ 3, 2, 1 },
		{ 2, 3, 0 }
	};

	myVertexInfo =
	{
		{
			{
				{ 1, 0 },
				{ 2, 3 },
				{ 3, 1 }
			}
		},
		{
			{
				{ 0, 1 },
				{ 2, 0 },
				{ 3, 2 }
			}
		},
		{
			{
				{ 0, 0 },
				{ 1, 2 },
				{ 3, 3 }
			}
		},
		{
			{
				{ 0, 3 },
				{ 1, 1 },
				{ 2, 2 }
			}
		}
	};

	myTotalVolumeDiscrepancy = 0;
}

bool Terrain::OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize)
{
	D3D11_MAPPED_SUBRESOURCE bufferData;
	WIPE(bufferData);

	HRESULT result = myFramework->GetContext()->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{
		SYSERROR("Could not map buffer");
		return false;
	}

	memcpy(bufferData.pData, aData, aSize);
	myFramework->GetContext()->Unmap(aBuffer, 0);

	return true;
}
