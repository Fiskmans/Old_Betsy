#include "pch.h"

#include "MarchingSquares.h"
#include "TextureFactory.h"
#include "AssetManager.h"
#include "Model.h"
#include "RenderScene.h"
#include "ShaderTypes.h"
#include "CommonUtilities/Intersection.hpp"

MarchingSquares::~MarchingSquares()
{
	SAFE_DELETE(myTexture);
}

bool MarchingSquares::Init(DirectX11Framework* aFamework)
{
	AddRandomOrb();

	myFamework = aFamework;

	myTexture = TextureFactory::GetInstance().CreateUpdatableTexture({ myResolution.x, myResolution.y }, "Marching_squares");

	if (!myTexture)
	{
		return false;
	}

	myTextureAsset = new TextureAsset(myTexture->GetResourceView());

	Model* model = new Model();
	myModelData = new Model::ModelData();

	myModelData->myshaderTypeFlags = ShaderFlags::None;
	myModelData->myStride = sizeof(Vertex);
	myModelData->myOffset = M44f::Identity();

	myModelData->myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	myModelData->myIndexBufferFormat = DXGI_FORMAT_R32_UINT;

	myModelData->myVertexShader = AssetManager::GetInstance().GetVertexShader("Model.hlsl", ShaderFlags::HasUvSets);
	myModelData->myPixelShader = AssetManager::GetInstance().GetPixelShader("PassThrough.hlsl", ShaderFlags::HasUvSets);

	myModelData->myTextures[0] = AssetHandle(myTextureAsset);

	SetupGraphicsResources();

	myModelData->myUseForwardRenderer = true;

	model->AddModelPart(myModelData);
	myModelAsset = new ModelAsset(model, "marching squares");
	myModelHandle = AssetHandle(myModelAsset);

	model->MarkLoaded();

	myModelInstance = myModelHandle.InstansiateModel();

	RenderScene::GetInstance().AddToScene(myModelInstance);

	myLastTimeStamp = Tools::GetTotalTime();

	return true;
}

void MarchingSquares::Update()
{
	if (myTexture)
	{
		float now = Tools::GetTotalTime();
		float dt = now - myLastTimeStamp;
		myLastTimeStamp = now;

		if (myMove)
		{
			DoMovement(dt * mySpeed);
		}
		GenerateGeometryData();
		GenerateGeometry();
	}
}

void MarchingSquares::DoMovement(float aDt)
{
	for (Orb& orb : myOrbs)
	{
		orb.myPosition += orb.mySpeed * aDt;
		while (true)
		{
			if (orb.myPosition.x < 0.f)
			{
				orb.myPosition.x *= -1.f;
				orb.mySpeed.x *= -1.f;
				continue;
			}
			if (orb.myPosition.x > 1.f)
			{
				orb.myPosition.x -= 1.f;
				orb.myPosition.x *= -1.f;
				orb.myPosition.x += 1.f;
				orb.mySpeed.x *= -1.f;
				continue;
			}
			if (orb.myPosition.y < 0.f)
			{
				orb.myPosition.y *= -1.f;
				orb.mySpeed.y *= -1.f;
				continue;
			}
			if (orb.myPosition.y > 1.f)
			{
				orb.myPosition.y -= 1.f;
				orb.myPosition.y *= -1.f;
				orb.myPosition.y += 1.f;
				orb.mySpeed.y *= -1.f;
				continue;
			}
			break;
		}
	}
}

void MarchingSquares::GenerateGeometryData()
{
	myTexture->GenerateAllPixels([this](unsigned int aX, unsigned int aY) -> float {
		//if (aX == 0 || aY == 0)
		//{
		//	return 0.f;
		//}
		//if (aX == myResolution.x-1 || aY == myResolution.y-1)
		//{
		//	return 0.f;
		//}


		CommonUtilities::Vector2<float> vec(static_cast<float>(aX) / static_cast<float>(myResolution.x), static_cast<float>(aY) / static_cast<float>(myResolution.y));
		float weight = 0;
		for (Orb& orb : myOrbs)
		{
			weight += (1.f / (orb.myPosition - vec).LengthSqr()) * orb.mySize;
		}

		weight /= myTheshold;

		return weight;
	});
	myTexture->Upload();
}

void MarchingSquares::GenerateGeometry()
{
	const V3F cellSize = V3F(mySize / myResolution.x, 0, mySize / myResolution.x);

	//static const V4F colors[16] =
	//{
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f),
	//	V4F(Tools::RandomDirection(),1.f)
	//};

	//for (unsigned int y = 0; y < myResolution.y; y++)
	//{
	//	for (unsigned int x = 0; x < myResolution.x; x++)
	//	{
	//		float weight = myTexture->Sample(x, y);
	//		if (weight > 1.f)
	//		{
	//			DebugDrawer::GetInstance().SetColor(V4F(1, 0, 0, 1));
	//		}
	//		else
	//		{
	//			DebugDrawer::GetInstance().SetColor(V4F(0, 0, 0, 1));
	//		}
	//
	//		V3F point = 
	//			V3F(-mySize / 2.f, myHeight, -mySize / 2.f) 
	//			+ V3F(mySize / static_cast<float>(myResolution.x) * x, 0, mySize / static_cast<float>(myResolution.y) * y) 
	//			+ cellSize / 2.f;
	//
	//		DebugDrawer::GetInstance().DrawLine(point, point + V3F(0, 1_m * CLAMP(0, 5, weight), 0));
	//	}
	//}
	const unsigned char lineCountMapping[16] =
	{
		0,
		1,
		1,
		1,

		1,
		1,
		2,
		1,

		1,
		2,
		1,
		1,

		1,
		1,
		1,
		0
	};

	struct Line
	{
		unsigned char myIndex1;
		unsigned char myIndex2;
	};


	//   1------------2                 
	//   |     0      |                   
	//   |            |                   
	//   |            |                   
	//   |1          3|                   
	//   |            |                   
	//   |      2     |                   
	//   4------------8                            

	const Line lineMappings[16][2]
	{
		{{0, 0}, {0, 0}}, // 0
		{{0, 1}, {0, 0}}, // 1
		{{0, 3}, {0, 0}}, // 2
		{{1, 3}, {0, 0}}, // 3
						  	 
		{{1, 2}, {0, 0}}, // 4
		{{0, 2}, {0, 0}}, // 5
		{{0, 3}, {1, 2}}, // 6
		{{2, 3}, {0, 0}}, // 7
						  	 
		{{2, 3}, {0, 0}}, // 8
		{{2, 3}, {0, 1}}, // 9
		{{0, 2}, {0, 0}}, // 10
		{{1, 2}, {0, 0}}, // 11
						  	 
		{{1, 3}, {0, 0}}, // 12
		{{0, 3}, {0, 0}}, // 13
		{{0, 1}, {0, 0}}, // 14
		{{0, 0}, {0, 0}}  // 15
	};

	DebugDrawer::GetInstance().SetColor(V4F(0.7f,1.f,0.7f,1.f));
	for (unsigned int y = 0; y < myResolution.y - 1; y++)
	{
		for (unsigned int x = 0; x < myResolution.x - 1; x++)
		{
			float weigths[4] = {
				myTexture->Sample(x + 0, y + 0),
				myTexture->Sample(x + 1, y + 0),
				myTexture->Sample(x + 0, y + 1),
				myTexture->Sample(x + 1, y + 1)
			};
			uint32_t index =
				((weigths[0] > 1.f) << 0)
				| ((weigths[1] > 1.f) << 1)
				| ((weigths[2] > 1.f) << 2)
				| ((weigths[3] > 1.f) << 3);

			if (index == 0b0000 || index == 0b1111)
			{
				continue; // edge does not go through this cell
			}

			V3F point =
				V3F(-mySize / 2.f, myHeight + 50_dm, -mySize / 2.f)
				+ V3F(mySize / static_cast<float>(myResolution.x) * x, 0, mySize / static_cast<float>(myResolution.y) * y)
				+ cellSize / 2.f;

			V3F points[4] =
			{
				point + V3F(cellSize.x * INVERSELERP(weigths[0], weigths[1], 1.f), 0, 0),
				point + V3F(0, 0, cellSize.z * INVERSELERP(weigths[0], weigths[2], 1.f)),
				point + V3F(cellSize.x * INVERSELERP(weigths[2], weigths[3], 1.f), 0, cellSize.z),
				point + V3F(cellSize.x, 0, cellSize.z * INVERSELERP(weigths[1], weigths[3], 1.f)),
			};

			for (size_t i = 0; i < lineCountMapping[index]; i++)
			{
				V3F from = points[lineMappings[index][i].myIndex1];
				V3F to = points[lineMappings[index][i].myIndex2];
				DebugDrawer::GetInstance().DrawLine(from, to);
			}
			//DebugDrawer::GetInstance().DrawLine(point, point + V3F(cellSize.x, 0, 0));
			//DebugDrawer::GetInstance().DrawLine(point, point + V3F(0, 0, cellSize.z));
			//DebugDrawer::GetInstance().DrawLine(point + cellSize, point + V3F(cellSize.x, 0, 0));
			//DebugDrawer::GetInstance().DrawLine(point + cellSize, point + V3F(0, 0, cellSize.z));
		}
	}
}

void MarchingSquares::SetupGraphicsResources()
{
	const size_t MaxTriCount = 2 * (myResolution.x - 1) * (myResolution.y - 1);

	ID3D11Device* device = myFamework->GetDevice();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* inputLayout;

	{
		CD3D11_BUFFER_DESC vertexBufferDescription;
		WIPE(vertexBufferDescription);
		vertexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(Vertex) * MaxTriCount * 3);
		vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


		std::vector<Vertex> initialVertex;
		initialVertex.resize(MaxTriCount * 3);
		initialVertex[0] = { V4F(-mySize / 2, myHeight, -mySize / 2, 1), V4F(0,1,0,0), V4F(1,0,0,0), V4F(0,0,1,0), CommonUtilities::Vector2<float>(0,0) };
		initialVertex[1] = { V4F(-mySize / 2, myHeight, +mySize / 2, 1), V4F(0,1,0,0), V4F(1,0,0,0), V4F(0,0,1,0), CommonUtilities::Vector2<float>(0,1) };
		initialVertex[2] = { V4F(+mySize / 2, myHeight, -mySize / 2, 1), V4F(0,1,0,0), V4F(1,0,0,0), V4F(0,0,1,0), CommonUtilities::Vector2<float>(1,0) };
		initialVertex[3] = { V4F(+mySize / 2, myHeight, +mySize / 2, 1), V4F(0,1,0,0), V4F(1,0,0,0), V4F(0,0,1,0), CommonUtilities::Vector2<float>(1,1) };

		D3D11_SUBRESOURCE_DATA initialData;
		WIPE(initialData);
		initialData.pSysMem = initialVertex.data();
		initialData.SysMemPitch = sizeof(Vertex);
		initialData.SysMemSlicePitch = sizeof(Vertex);

		HRESULT result = device->CreateBuffer(&vertexBufferDescription, &initialData, &vertexBuffer);
		if (FAILED(result))
		{
			SYSERROR("Couldn not create vertex buffer", "MarchingSquares");
			return;
		}
	}

	Tools::ExecuteOnDestruct releaseVertexBuffer = Tools::ExecuteOnDestruct([vertexBuffer]() { vertexBuffer->Release(); });

	{

		CD3D11_BUFFER_DESC indexBufferDescription;
		WIPE(indexBufferDescription);
		indexBufferDescription.ByteWidth = static_cast<UINT>(MaxTriCount * sizeof(Tri));
		indexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		std::vector<Tri> initialTris;
		initialTris.resize(MaxTriCount);
		initialTris[0] = { 0,1,2 };
		initialTris[1] = { 2,1,3 };

		D3D11_SUBRESOURCE_DATA initialData;
		WIPE(initialData);
		initialData.pSysMem = initialTris.data();
		initialData.SysMemPitch = sizeof(Tri);
		initialData.SysMemSlicePitch = sizeof(Tri);

		HRESULT result = device->CreateBuffer(&indexBufferDescription, &initialData, &indexBuffer);
		if (FAILED(result))
		{
			SYSERROR("Could not create index buffer", "MarchingSquares");
			return;
		}
	}

	Tools::ExecuteOnDestruct releaseIndexBuffer = Tools::ExecuteOnDestruct([indexBuffer]() { indexBuffer->Release(); });

	{
		D3D11_INPUT_ELEMENT_DESC layout[ShaderTypes::MaxInputElementSize];
		UINT layoutElements = ShaderTypes::InputLayoutFromFlags(layout, ShaderFlags::HasUvSets);

		HRESULT result = device->CreateInputLayout(layout, layoutElements, myModelData->myVertexShader.GetVertexShaderblob().data(), myModelData->myVertexShader.GetVertexShaderblob().size(), &inputLayout);
		if (FAILED(result))
		{
			SYSERROR("Could not create inputlayout", "MarchingSquares");
			return;
		}
	}

	Tools::ExecuteOnDestruct releaseInputLayout = Tools::ExecuteOnDestruct([inputLayout]() { inputLayout->Release(); });

	myModelData->myVertexBuffer = vertexBuffer;
	myModelData->myIndexBuffer = indexBuffer;
	myModelData->myInputLayout = inputLayout;
	myModelData->myNumberOfIndexes = 6;
	myModelData->myUseForwardRenderer = true;

	releaseVertexBuffer.Disable();
	releaseIndexBuffer.Disable();
	releaseInputLayout.Disable();
}

#if USEIMGUI
void MarchingSquares::Imgui()
{
	WindowControl::Window("Marching Squares", [this]()
	{
		ImGui::Checkbox("Do Movement", &myMove);
		ImGui::SameLine();
		if (ImGui::Button("AddOrb"))
		{
			AddRandomOrb();
		}
		ImGui::InputFloat("Speed", &mySpeed);
		ImGui::InputFloat("Threshold", &myTheshold);
		ImGui::Separator();

		for (int i = 0; i < myOrbs.size(); i++)
		{
			Orb& orb = myOrbs[i];

			ImGui::PushID(&orb);
			if (ImGui::TreeNode("Orb"))
			{
				ImGui::InputFloat("Size", &orb.mySize);
				ImGui::InputFloat2("Velocity", &orb.mySpeed.x);
				ImGui::InputFloat2("Position", &orb.myPosition.x);
				if (ImGui::Button("Remove"))
				{
					myOrbs.erase(myOrbs.begin() + i);
					break;
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	});
}
#endif

void MarchingSquares::AddRandomOrb()
{
	V3F dir = Tools::RandomDirection();
	myOrbs.emplace_back(Orb{
			CommonUtilities::Vector2<float>(Tools::RandomNormalized(),Tools::RandomNormalized()),
			CommonUtilities::Vector2<float>(dir.x, dir.y), // flattening away z gives us a distribution that favors slower orbs 
			Tools::RandomNormalized() * Tools::RandomNormalized() * 0.5f
		});
}