#include "pch.h"
#include "Terrain.h"

#include "GamlaBettan\DirectX11Framework.h"
#include "GamlaBettan\AssetManager.h"
#include "GamlaBettan\Model.h"
#include "GamlaBettan\Scene.h"

#define TERRAIN_FILE_VERSION (1)

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
		if (!OverWriteBuffer(myModelData->myVertexBuffer, myVertexes.data(), myVertexes.size() * sizeof(Vertex)))
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

			const static V4F colors[3] =
			{
				V4F(1,0.2,0.2,1),
				V4F(0.2,1,0.2,1),
				V4F(0.2,0.2,1,1),
			};

			ImGui::Text("Total volume change: %f", myTotalVolumeDiscrepancy);
			ImGui::Checkbox("Show Normals", &showNormals);
			if (ImGui::Button("Tesselate biggest"))
			{
				float minSize = 0.f;
				UINT selected = 0;
				for (UINT i = 0; i < myTris.size(); i++)
				{
					float size = Area(i);
					if (size > minSize)
					{
						minSize = size;
						selected = i;
					}
				}
				Tesselate(selected);
			}

			if (ImGui::TreeNode("Mesh"))
			{
				if (ImGui::TreeNode("Tris"))
				{
					for (size_t i = 0; i < myTris.size(); i++)
					{
						Tri& tri = myTris[i];

						ImGui::PushID(i);
						bool open = ImGui::TreeNode("tri", "Tri #%d", i);
						if (ImGui::IsItemHovered())
						{
							DebugDrawer::GetInstance().SetColor(colors[0]);
							DebugDrawer::GetInstance().DrawCross(myVertexes[tri.myA].myPosition, 10_cm);
							DebugDrawer::GetInstance().DrawLine(myVertexes[tri.myA].myPosition, myVertexes[tri.myC].myPosition);
							for (VertexInfo::Edge& edge : myVertexInfo[tri.myA].myEdges)
							{
								if (edge.myOtherVertex == tri.myC)
								{
									UINT other = tri.myA ^ tri.myC ^ myTris[edge.myTri].myA ^ myTris[edge.myTri].myB ^ myTris[edge.myTri].myC;
									V3F center = (myVertexes[tri.myA].myPosition + myVertexes[tri.myC].myPosition) / 2.f;
									DebugDrawer::GetInstance().DrawDirection(
										center,
										myVertexes[other].myPosition - center,
										20_cm
									);
								}
							}

							DebugDrawer::GetInstance().SetColor(colors[1]);
							DebugDrawer::GetInstance().DrawCross(myVertexes[tri.myB].myPosition, 10_cm);
							DebugDrawer::GetInstance().DrawLine(myVertexes[tri.myB].myPosition, myVertexes[tri.myA].myPosition);
							for (VertexInfo::Edge& edge : myVertexInfo[tri.myB].myEdges)
							{
								if (edge.myOtherVertex == tri.myA)
								{
									UINT other = tri.myB ^ tri.myA ^ myTris[edge.myTri].myA ^ myTris[edge.myTri].myB ^ myTris[edge.myTri].myC;
									V3F center = (myVertexes[tri.myB].myPosition + myVertexes[tri.myA].myPosition) / 2.f;
									DebugDrawer::GetInstance().DrawDirection(
										center,
										myVertexes[other].myPosition - center,
										20_cm
									);
								}
							}

							DebugDrawer::GetInstance().SetColor(colors[2]);
							DebugDrawer::GetInstance().DrawCross(myVertexes[tri.myC].myPosition, 10_cm);
							DebugDrawer::GetInstance().DrawLine(myVertexes[tri.myC].myPosition, myVertexes[tri.myB].myPosition);
							for (VertexInfo::Edge& edge : myVertexInfo[tri.myC].myEdges)
							{
								if (edge.myOtherVertex == tri.myB)
								{
									UINT other = tri.myC ^ tri.myB ^ myTris[edge.myTri].myA ^ myTris[edge.myTri].myB ^ myTris[edge.myTri].myC;
									V3F center = (myVertexes[tri.myC].myPosition + myVertexes[tri.myB].myPosition) / 2.f;
									DebugDrawer::GetInstance().DrawDirection(
										center,
										myVertexes[other].myPosition - center,
										20_cm
									);
								}
							}
						}
						if (open)
						{
							ImGui::Text("Area: %f", Area(i));
							ImGui::Text("Vertcies %d %d %d", tri.myA, tri.myB, tri.myC);
							if (ImGui::Button("Tesselate"))
							{
								Tesselate(i);
							}

							ImGui::TreePop();
						}

						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Verticies"))
				{
					for (size_t i = 0; i < myVertexes.size(); i++)
					{
						ImGui::PushID(i);

						V3F pos = myVertexes[i].myPosition;
						bool hovered = false;
						if (Tools::EditPosition("Vertex", &pos.x, hovered))
						{
							MoveVertex(i, pos);
						}

						if (hovered)
						{
							static V4F colors[] =
							{
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f)),
								Tools::RandomRange(V4F(0.4f, 0.4f, 0.4f, 1.f), V4F(1.f, 1.f, 1.f, 1.f))
							};

							DebugDrawer::GetInstance().SetColor(V4F(1.f, 0.f, 0.f, 1.f));
							DebugDrawer::GetInstance().DrawCross(myVertexes[i].myPosition, 20_cm);

							for (size_t edgeIndex = 0; edgeIndex < myVertexInfo[i].myEdges.size(); edgeIndex++)
							{
								DebugDrawer::GetInstance().SetColor(colors[edgeIndex % C_ARRAY_SIZE(colors)]);

								VertexInfo::Edge& edge = myVertexInfo[i].myEdges[edgeIndex];
								DebugDrawer::GetInstance().DrawArrow(myVertexes[i].myPosition, myVertexes[edge.myOtherVertex].myPosition);
								DebugDrawer::GetInstance().DrawCross(
									(myVertexes[myTris[edge.myTri].myA].myPosition +
										myVertexes[myTris[edge.myTri].myB].myPosition +
										myVertexes[myTris[edge.myTri].myC].myPosition) / 3.f,
									10_cm );
							}

						}

						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			if (showNormals)
			{
				DebugDrawer::GetInstance().SetColor(V4F(0, 0.5, 0, 1));
				for (Vertex& vertex : myVertexes)
				{
					DebugDrawer::GetInstance().DrawDirection(vertex.myPosition, vertex.myNormal);
				}
			}
		});
}
#endif

void Terrain::FromFile(const std::string& aFilePath)
{
	std::string ext = Tools::ExtensionFromPath(aFilePath);

	if (ext == ".fbx")
	{
		FromFBX(aFilePath);
		return;
	}
	else if (ext == ".ter")
	{
		FromTER(aFilePath);
		return;
	}
	else
	{
		SYSERROR("Terrain does not know how to load files of this type", aFilePath);
	}
}

void Terrain::FromFBX(const std::string aFilePath)
{
}

void Terrain::FromTER(const std::string aFilePath)
{
	std::vector<char> fileContent = Tools::ReadWholeFileBinary(aFilePath);

	uint32_t programVersion = TERRAIN_FILE_VERSION;

	if (fileContent.size() < sizeof(programVersion))
	{
		SYSERROR("Terrian file corrupt", aFilePath);
		return;
	}

	uint32_t version = *reinterpret_cast<uint32_t*>(fileContent.data());
	uint32_t offset = sizeof(version);

	if (version != programVersion)
	{
		SYSERROR("Terrain file outdated", aFilePath, std::to_string(version));
		return;
	}

	if (fileContent.size() < offset + sizeof(float))
	{
		SYSERROR("Terrian file corrupt", aFilePath);
		return;
	}

	myTotalVolumeDiscrepancy = *reinterpret_cast<float*>(fileContent.data() + offset);
	offset += sizeof(myTotalVolumeDiscrepancy);

	if (fileContent.size() < offset + sizeof(uint32_t))
	{
		SYSERROR("Terrian file corrupt", aFilePath);
		return;
	}

	uint32_t vertexCount = *reinterpret_cast<uint32_t*>(fileContent.data() + offset);
	offset += sizeof(vertexCount);

	if (fileContent.size() < offset + sizeof(Vertex) * vertexCount)
	{
		SYSERROR("Terrian file corrupt", aFilePath);
		return;
	}

	myVertexes.resize(vertexCount);
	memcpy(myVertexes.data(), fileContent.data() + offset, sizeof(Vertex) * vertexCount);
	offset += sizeof(Vertex) * vertexCount;

	if (fileContent.size() < offset + sizeof(uint32_t))
	{
		SYSERROR("Terrian file corrupt", aFilePath);
		return;
	}

	uint32_t triCount = *reinterpret_cast<uint32_t*>(fileContent.data() + offset);
	offset += sizeof(triCount);

	if (fileContent.size() < offset + sizeof(Tri) * triCount)
	{
		SYSERROR("Terrian file corrupt", aFilePath);
		return;
	}

	myTris.resize(triCount);
	memcpy(myTris.data(), fileContent.data() + offset, sizeof(Tri) * triCount);
	offset += sizeof(Tri) * triCount;

	if (offset < fileContent.size())
	{
		SYSWARNING("Unconsumed data in terrain file", aFilePath, Tools::PrintByteSize(fileContent.size() - offset));
	}


	GenerateVertexInfo();
	SetupAllNormals();
}

void Terrain::GenerateVertexInfo()
{
	myVertexInfo.resize(0);
	myVertexInfo.resize(myVertexes.size());
	for (UINT i = 0; i < myTris.size(); i++)
	{
		Tri& tri = myTris[i];
	
		myVertexInfo[tri.myA].myEdges.push_back({
				tri.myB,
				i
			});

		myVertexInfo[tri.myB].myEdges.push_back({
				tri.myC,
				i
			});

		myVertexInfo[tri.myC].myEdges.push_back({
				tri.myA,
				i
			});
	}
}

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
		V3F totalNormal = V3F(0, 0, 0);
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

float Terrain::Area(UINT aTri)
{
	Tri& tri = myTris[aTri];
	return (myVertexes[tri.myA].myPosition - myVertexes[tri.myB].myPosition).Cross(
		(myVertexes[tri.myA].myPosition - myVertexes[tri.myC].myPosition)).Length() / 2.f;
}

float Terrain::Triangleness(UINT aTri)
{
	return 0.0f;
}

void Terrain::TestMesh()
{
	const float size = 5_m;
	myVertexes =
	{
		{ V3F(-size / 2,	 0,		 size / 2),	V3F(-1,  0,  0) }, // left
		{ V3F(size / 2,	 0,		 size / 2),	V3F(1,  0,  0) }, // right
		{ V3F(0,			 0,		-size / 2), V3F(0,  0, -1) }, // front
		{ V3F(0,			-size,   0),		V3F(0, -1,  0) }  // down
	};

	myTris =
	{
		{ 0, 1, 2 },
		{ 1, 0, 3 },
		{ 3, 2, 1 },
		{ 2, 3, 0 }
	};

	myTotalVolumeDiscrepancy = 0;

	GenerateVertexInfo();
}

void Terrain::MoveVertex(UINT aIndex, V3F aTargetPosition)
{
	V3F delta = aTargetPosition - myVertexes[aIndex].myPosition;
	float totalDiff = 0;
	for (VertexInfo::Edge& edge : myVertexInfo[aIndex].myEdges)
	{
		Tri& tri = myTris[edge.myTri];
		V3F cross = (myVertexes[tri.myA].myPosition - myVertexes[tri.myB].myPosition).Cross(
			(myVertexes[tri.myA].myPosition - myVertexes[tri.myC].myPosition));

		totalDiff += delta.Dot(cross);
	}
	const float scale = 1.f / (1_m * 1_m * 1_m * 6);
	myTotalVolumeDiscrepancy += totalDiff * scale;

	myVertexes[aIndex].myPosition = aTargetPosition;
	SetupNormals(aIndex);
}

void Terrain::Tesselate(UINT aTri)
{
	Tri& tri = myTris[aTri];
	std::vector<UINT> dirtyVerticies = 
	{
		tri.myA,
		tri.myB,
		tri.myC,
		AddVertex((myVertexes[tri.myA].myPosition + myVertexes[tri.myC].myPosition) / 2.f),
		AddVertex((myVertexes[tri.myB].myPosition + myVertexes[tri.myA].myPosition) / 2.f),
		AddVertex((myVertexes[tri.myC].myPosition + myVertexes[tri.myB].myPosition) / 2.f),
		0,
		0,
		0
	};


	tri.myA = dirtyVerticies[3];
	tri.myB = dirtyVerticies[4];
	tri.myC = dirtyVerticies[5];

	Tri triCopy = tri;

	UINT newTris[6] =
	{
		AddTri(triCopy.myA, triCopy.myC, dirtyVerticies[2]),
		AddTri(triCopy.myB, triCopy.myA, dirtyVerticies[0]),
		AddTri(triCopy.myC, triCopy.myB, dirtyVerticies[1]),
		0,
		0,
		0
	};

	UINT oldTris[3] =
	{
		0,
		0,
		0
	};


	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[0]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[2])
		{
			oldTris[0] = edge.myTri;
			dirtyVerticies[6] = dirtyVerticies[0] ^ dirtyVerticies[2] ^ XorAllVerticies(edge.myTri);
			newTris[3] = AddTri(dirtyVerticies[3], dirtyVerticies[2], dirtyVerticies[6]);
			break;
		}
	}

	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[1]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[0])
		{
			oldTris[1] = edge.myTri;
			dirtyVerticies[7] = dirtyVerticies[1] ^ dirtyVerticies[0] ^ XorAllVerticies(edge.myTri);
			newTris[4] = AddTri(dirtyVerticies[4], dirtyVerticies[0], dirtyVerticies[7]);
			break;
		}
	}

	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[2]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[1])
		{
			oldTris[2] = edge.myTri;
			dirtyVerticies[8] = dirtyVerticies[2] ^ dirtyVerticies[1] ^ XorAllVerticies(edge.myTri);
			newTris[5] = AddTri(dirtyVerticies[5], dirtyVerticies[1], dirtyVerticies[8]);
			break;
		}
	}


	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[0]].myEdges)
	{
		if (edge.myTri == oldTris[1])
		{
			edge.myTri = newTris[4];
			break;
		}
	}
	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[1]].myEdges)
	{
		if (edge.myTri == oldTris[2])
		{
			edge.myTri = newTris[5];
			break;
		}
	}

	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[2]].myEdges)
	{
		if (edge.myTri == oldTris[0])
		{
			edge.myTri = newTris[3];
			break;
		}
	}

	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[0]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[2])
		{
			SwapVertex(edge.myTri, dirtyVerticies[2], dirtyVerticies[3]);
			edge.myOtherVertex = dirtyVerticies[3];
			break;
		}
	}
	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[2]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[0])
		{
			edge.myOtherVertex = dirtyVerticies[3];
			edge.myTri = newTris[0];
			break;
		}
	}
	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[1]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[0])
		{
			SwapVertex(edge.myTri, dirtyVerticies[0], dirtyVerticies[4]);
			edge.myOtherVertex = dirtyVerticies[4];
			break;
		}
	}
	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[0]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[1])
		{
			edge.myOtherVertex = dirtyVerticies[4];
			edge.myTri = newTris[1];
			break;
		}
	}
	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[2]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[1])
		{
			SwapVertex(edge.myTri, dirtyVerticies[1], dirtyVerticies[5]);
			edge.myOtherVertex = dirtyVerticies[5];
			break;
		}
	}
	for (VertexInfo::Edge& edge : myVertexInfo[dirtyVerticies[1]].myEdges)
	{
		if (edge.myOtherVertex == dirtyVerticies[2])
		{
			edge.myOtherVertex = dirtyVerticies[5];
			edge.myTri = newTris[2];
			break;
		}
	}

	myVertexInfo[dirtyVerticies[6]].myEdges.push_back(
		{
			dirtyVerticies[3],
			newTris[3]
		}
	);
	myVertexInfo[dirtyVerticies[7]].myEdges.push_back(
		{
			dirtyVerticies[4],
			newTris[4]
		}
	);
	myVertexInfo[dirtyVerticies[8]].myEdges.push_back(
		{
			dirtyVerticies[5],
			newTris[5]
		}
	);

	myVertexInfo[dirtyVerticies[3]].myEdges =
	{
		{
			dirtyVerticies[6],
			oldTris[0]
		},
		{
			dirtyVerticies[0],
			newTris[1]
		},
		{
			dirtyVerticies[4],
			aTri
		},
		{
			dirtyVerticies[5],
			newTris[0]
		},
		{
			dirtyVerticies[2],
			newTris[3]
		}
	};

	myVertexInfo[dirtyVerticies[4]].myEdges =
	{
		{
			dirtyVerticies[7],
			oldTris[1]
		},
		{
			dirtyVerticies[1],
			newTris[2]
		},
		{
			dirtyVerticies[5],
			aTri
		},
		{
			dirtyVerticies[3],
			newTris[1]
		},
		{
			dirtyVerticies[0],
			newTris[4]
		}
	};

	myVertexInfo[dirtyVerticies[5]].myEdges =
	{
		{
			dirtyVerticies[8],
			oldTris[2]
		},
		{
			dirtyVerticies[2],
			newTris[0]
		},
		{
			dirtyVerticies[3],
			aTri
		},
		{
			dirtyVerticies[4],
			newTris[2]
		},
		{
			dirtyVerticies[1],
			newTris[5]
		}
	};

	SetupNormals(dirtyVerticies);
}

void Terrain::SwapVertex(UINT aTri, UINT aOriginal, UINT aTarget)
{
	Tri& tri = myTris[aTri];

	if (tri.myA == aOriginal)
	{
		tri.myA = aTarget;
		return;
	}
	if (tri.myB == aOriginal)
	{
		tri.myB = aTarget;
		return;
	}
	if (tri.myC == aOriginal)
	{
		tri.myC = aTarget;
		return;
	}
	throw std::exception("Original vertex was not part of tri");
}

UINT Terrain::XorAllVerticies(UINT aTri)
{
	Tri& tri = myTris[aTri];
	return tri.myA ^ tri.myB ^ tri.myC;
}

UINT Terrain::AddVertex(V3F aPosition)
{
	UINT number = myVertexes.size();
	myVertexes.push_back(
		{
			aPosition,
			V3F()
		}
	);
	myVertexInfo.push_back({});
	return number;
}

UINT Terrain::AddTri(UINT aA, UINT aB, UINT aC)
{
	UINT number = myTris.size();
	myTris.push_back(
		{
			aA,
			aB,
			aC
		}
	);
	return number;
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
