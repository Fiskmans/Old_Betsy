#include "pch.h"
#include "Terrain.h"

#include "GamlaBettan\DirectX11Framework.h"
#include "GamlaBettan\AssetManager.h"
#include "GamlaBettan\Model.h"
#include "GamlaBettan\RenderScene.h"
#include "MarchingCubes.h"

#define TERRAIN_FILE_VERSION (1)

Terrain::Terrain(DirectX11Framework* aFramework, GBPhysX* aPhysx, const CommonUtilities::Vector3<size_t>& aResolution, const V3F aSize)
{
	myFramework = aFramework;
	myPhysx = aPhysx;

	myResolution = aResolution;
	myIndexing = { 1, myResolution.x * myResolution.y, myResolution.x };
	myTerrainData.resize(myResolution.Volume());

	MaxTriCount = (myResolution - CommonUtilities::Vector3<size_t>{ 1, 1, 1 }).Volume() * 5;
	mySize = aSize;
	Setup();
	GenerateTerrain();
}

void Terrain::Update()
{
	float now = Tools::GetTotalTime();

	if (myIsDirty && now > myLastRebake + myRebakeInterval)
	{
		myVertexes.clear();
		uint32_t baseSeed = 0;
		uint32_t seed = 0;
		for (auto& tri : math::MarchingCubes(myTerrainData.begin(), myIndexing, myResolution, -1.f))
		{

			V3F a = tri.myA * mySize;
			V3F b = tri.myB * mySize;
			V3F c = tri.myC * mySize;

			V3F ab = a - b;
			V3F ac = a - c;
			V3F normal = -ab.Cross(ac).GetNormalized();

			V3F middle = (a + b + c) / 3.f;
			uint32_t s = static_cast<uint32_t>(((middle / mySize * myResolution.As<float>()).As<size_t>() * myIndexing).Sum() * 5);
			if (baseSeed != s)
			{
				baseSeed = s;
				seed = s;
			}
			else
			{
				seed++;
			}

			myVertexes.push_back(Vertex{ a, normal, seed });
			myVertexes.push_back(Vertex{ b, normal, seed });
			myVertexes.push_back(Vertex{ c, normal, seed });
		}

		myLastRebake = now;

		if (!OverWriteBuffer(myModelData->myVertexBuffer, myVertexes.data(), myVertexes.size() * sizeof(Vertex)))
		{
			RenderScene::GetInstance().RemoveFromScene(myModelInstance);
			return;
		}
		myModelData->myNumberOfVertexes = static_cast<UINT>(myVertexes.size());

		if (myActor)
		{
			myActor->RemoveFromScene();
			myActor->Release();
			delete myActor;
		}

		myActor = myPhysx->GBCreateTriangleMesh(
			reinterpret_cast<const char*>(myVertexes.data()),
			sizeof(Vertex),
			myVertexes.size());

		myIsDirty = false;
	}
}

#if USEIMGUI
void Terrain::Imgui()
{
	WindowControl::Window("Terrain", [this]()
	{
		ImguiContent();
	});
}

void Terrain::ImguiContent()
{
	static bool showNormals;

	const static V4F colors[3] =
	{
		V4F(1.f,0.2f,0.2f,1.f),
		V4F(0.2f,1.f,0.2f,1.f),
		V4F(0.2f,0.2f,1.f,1.f),
	};

	if (ImGui::Button("Generate"))
	{
		GenerateTerrain();
	}

	ImGui::Checkbox("Show Normals", &showNormals);
	ImGui::Button("+");
	if (ImGui::IsItemHovered() && ImGui::GetIO().MouseDown[0])
	{
		for (Vertex& vertex : myVertexes)
		{
			vertex.myPosition *= 1.01f;
		}
		myIsDirty = true;
	}
	ImGui::SameLine();
	ImGui::Button("-");
	if (ImGui::IsItemHovered() && ImGui::GetIO().MouseDown[0])
	{
		for (Vertex& vertex : myVertexes)
		{
			vertex.myPosition /= 1.01f;
		}
		myIsDirty = true;
	}
	//UINT selectedTri = static_cast<UINT>(-1);

	static bool EPressed = false;
	if (EPressed)
	{
		if (!GetAsyncKeyState('E'))
		{
			EPressed = false;
		}
	}
	else
	{
		if (GetAsyncKeyState('E'))
		{
			HitResult report = myPhysx->RayCast(Input::GetInstance().MouseRay(), 50_m);
			if (report.actor == myActor)
			{
				Dig(report.position, 100);
			}

			EPressed = true;
		}
	}
	static FRay ray = Input::GetInstance().MouseRay();
	if (GetAsyncKeyState('K'))
	{
		ray = Input::GetInstance().MouseRay();
	}
	DebugDrawer::GetInstance().DrawDirection(ray.Position(), ray.Direction(), 5_m);

	ImGui::Checkbox("isDirty", &myIsDirty);

	static std::vector<int> badTris;
	if (ImGui::TreeNode("bad tris"))
	{
		if (ImGui::Button("Isolate bad"))
		{
			for (UINT i = 0; i < myVertexes.size(); i += 3)
			{
				GBPhysXActor* actor = myPhysx->GBCreateTriangleMesh(
					reinterpret_cast<const char*>(myVertexes.data() + i),
					sizeof(Vertex),
					3);

				if (!actor)
				{
					SYSERROR("Triangle is bad ", std::to_string(i / 3));
					badTris.push_back(i / 3);
				}
			}
		}
		if (ImGui::Button("Clear"))
		{
			badTris.clear();
		}
		for (int& b : badTris)
		{
			if (ImGui::TreeNode(std::to_string(b).c_str()))
			{
				ImGui::InputFloat3("a", &(myVertexes[b * 3 + 0].myPosition.x));
				ImGui::InputFloat3("b", &(myVertexes[b * 3 + 1].myPosition.x));
				ImGui::InputFloat3("c", &(myVertexes[b * 3 + 2].myPosition.x));
				ImGui::TreePop();
			}

			DebugDrawer::GetInstance().DrawLine(myVertexes[b * 3 + 0].myPosition, myVertexes[b * 3 + 1].myPosition);
			DebugDrawer::GetInstance().DrawLine(myVertexes[b * 3 + 0].myPosition, myVertexes[b * 3 + 2].myPosition);
			DebugDrawer::GetInstance().DrawLine(myVertexes[b * 3 + 1].myPosition, myVertexes[b * 3 + 2].myPosition);

			DebugDrawer::GetInstance().DrawDirection((myVertexes[b * 3 + 0].myPosition + myVertexes[b * 3 + 1].myPosition + myVertexes[b * 3 + 2].myPosition) / 3.f, myVertexes[b * 3 + 0].myNormal, 5_dm);

		}
		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Mesh"))
	{
		ImGui::Button("hover for unit cube");
		if (ImGui::IsItemHovered())
		{
			DebugDrawer::GetInstance().DrawLine({ 0,0,0 }, { 0,0,1 });
			DebugDrawer::GetInstance().DrawLine({ 0,0,0 }, { 0,1,0 });
			DebugDrawer::GetInstance().DrawLine({ 0,0,0 }, { 1,0,0 });

			DebugDrawer::GetInstance().DrawLine({ 0,0,1 }, { 0,1,1 });
			DebugDrawer::GetInstance().DrawLine({ 0,0,1 }, { 1,0,1 });

			DebugDrawer::GetInstance().DrawLine({ 0,1,0 }, { 1,1,0 });
			DebugDrawer::GetInstance().DrawLine({ 0,1,0 }, { 0,1,1 });

			DebugDrawer::GetInstance().DrawLine({ 1,0,0 }, { 1,1,0 });
			DebugDrawer::GetInstance().DrawLine({ 1,0,0 }, { 1,0,1 });

			DebugDrawer::GetInstance().DrawLine({ 1,1,1 }, { 1,1,0 });
			DebugDrawer::GetInstance().DrawLine({ 1,1,1 }, { 1,0,1 });
			DebugDrawer::GetInstance().DrawLine({ 1,1,1 }, { 0,1,1 });
		}

		ImGui::Button("hover for scaled unit cube");
		if (ImGui::IsItemHovered())
		{
			DebugDrawer::GetInstance().DrawLine(V3F{ 0,0,0 } *mySize, V3F{ 0,0,1 } *mySize);
			DebugDrawer::GetInstance().DrawLine(V3F{ 0,0,0 } *mySize, V3F{ 0,1,0 } *mySize);
			DebugDrawer::GetInstance().DrawLine(V3F{ 0,0,0 } *mySize, V3F{ 1,0,0 } *mySize);

			DebugDrawer::GetInstance().DrawLine(V3F{ 0,0,1 } *mySize, V3F{ 0,1,1 } *mySize);
			DebugDrawer::GetInstance().DrawLine(V3F{ 0,0,1 } *mySize, V3F{ 1,0,1 } *mySize);

			DebugDrawer::GetInstance().DrawLine(V3F{ 0,1,0 } *mySize, V3F{ 1,1,0 } *mySize);
			DebugDrawer::GetInstance().DrawLine(V3F{ 0,1,0 } *mySize, V3F{ 0,1,1 } *mySize);

			DebugDrawer::GetInstance().DrawLine(V3F{ 1,0,0 } *mySize, V3F{ 1,1,0 } *mySize);
			DebugDrawer::GetInstance().DrawLine(V3F{ 1,0,0 } *mySize, V3F{ 1,0,1 } *mySize);

			DebugDrawer::GetInstance().DrawLine(V3F{ 1,1,1 } *mySize, V3F{ 1,1,0 } *mySize);
			DebugDrawer::GetInstance().DrawLine(V3F{ 1,1,1 } *mySize, V3F{ 1,0,1 } *mySize);
			DebugDrawer::GetInstance().DrawLine(V3F{ 1,1,1 } *mySize, V3F{ 0,1,1 } *mySize);
		}


		if (ImGui::TreeNode("Vertexes"))
		{
			ImGui::Text("count: %u", myVertexes.size());
			ImGui::Separator();
			for (Vertex& vertex : myVertexes)
			{
				ImGui::PushID(&vertex);
				ImGui::InputFloat3("pos", &vertex.myPosition.x);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Tris"))
		{
			size_t triCount = myVertexes.size() / 3;
			ImGui::Text("count: %u", triCount);
			ImGui::Separator();
			for (size_t i = 0; i < triCount; i++)
			{
				DebugDrawer::GetInstance().DrawLine(myVertexes[i * 3 + 0].myPosition, myVertexes[i * 3 + 1].myPosition);
				DebugDrawer::GetInstance().DrawLine(myVertexes[i * 3 + 0].myPosition, myVertexes[i * 3 + 2].myPosition);
				DebugDrawer::GetInstance().DrawLine(myVertexes[i * 3 + 1].myPosition, myVertexes[i * 3 + 2].myPosition);

				DebugDrawer::GetInstance().DrawDirection((myVertexes[i * 3 + 0].myPosition + myVertexes[i * 3 + 1].myPosition + myVertexes[i * 3 + 2].myPosition) / 3.f, myVertexes[i * 3 + 0].myNormal, 5_dm);
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}


	/*
	if (selectedTri != -1)
	{
		ImGui::Text("Debugging tri: %d", selectedTri);
		Tri& tri = myTris[selectedTri];
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
	*/

	if (showNormals)
	{
		DebugDrawer::GetInstance().SetColor(V4F(0.f, 0.5f, 0.f, 1.f));
		for (Vertex& vertex : myVertexes)
		{
			DebugDrawer::GetInstance().DrawDirection(vertex.myPosition, vertex.myNormal);
		}
	}
}
#endif

Terrain::DigResult Terrain::Dig(V3F aPosition, size_t aPower)
{
	const CommonUtilities::Vector3<float> center = aPosition / mySize * myResolution.As<float>();

	const float range = std::pow(static_cast<float>(aPower), 1.f / 3.f) * 3_dm;
	const V3F spheroidScale = myResolution.As<float>() / mySize * range;

	DigResult result;

	float zMin = center.z - spheroidScale.z;
	float zMax = center.z + spheroidScale.z;

	for (float z = std::max(zMin, 0.f); z < zMax && z < myResolution.z; z += 1.f)
	{
		float zloss = std::cos(INVERSELERP(center.z, center.z + spheroidScale.z, z) * PI_F);
		float yMin = center.y - spheroidScale.y * zloss;
		float yMax = center.y + spheroidScale.y * zloss;

		for (float y = std::max(yMin, 0.f); y < yMax && y < myResolution.y; y += 1.f)
		{
			float yloss = std::cos(INVERSELERP(center.y, center.y + spheroidScale.y, y) * PI_F);
			float xMin = center.x - spheroidScale.x * zloss * yloss;
			float xMax = center.x + spheroidScale.x * zloss * yloss;

			for (float x = std::max(xMin, 0.f); x < xMax && x < myResolution.y; x += 1.f)
			{
				const V3F point{ x, y, z };
				const size_t power = static_cast<size_t>(std::ceil(1.f / (point - center).LengthSqr() * static_cast<float>(aPower)));

				TerrainNode& node = myTerrainData[(CommonUtilities::Vector3<int>{1, static_cast<int>(myResolution.x), static_cast<int>(myResolution.x* myResolution.y) } *point.As<int>()).Sum()];
				node.AttemptDig(result, aPower);
			}
		}
	}

	if (!result.Empty())
	{
		myIsDirty = true;
	}

	return result;
}

void Terrain::GenerateTerrain()
{
	for (size_t x = 0; x < myResolution.x; x++)
	{
		for (size_t z = 0; z < myResolution.z; z++)
		{
			float genY = 0.9f;
			float scaledY = genY * myResolution.y;
			size_t wholePart = static_cast<size_t>(scaledY);
			uint8_t fraction = static_cast<uint8_t>((scaledY - wholePart) * std::numeric_limits<uint8_t>::max());

			for (size_t y = 0; y < wholePart; y++)
			{
				TerrainNode& node = myTerrainData[(CommonUtilities::Vector3<size_t>{1, myResolution.x, myResolution.x * myResolution.y } * CommonUtilities::Vector3<size_t>{ x, y, z }).Sum()];

				node.myDirt = std::numeric_limits<decltype(node.myDirt)>::max();
				node.myRock = std::numeric_limits<decltype(node.myDirt)>::min();
				node.myIron = std::numeric_limits<decltype(node.myDirt)>::min();
				node.myGold = std::numeric_limits<decltype(node.myDirt)>::min();
			}

			{
				TerrainNode& node = myTerrainData[(CommonUtilities::Vector3<size_t>{1, myResolution.x, myResolution.x * myResolution.y } * CommonUtilities::Vector3<size_t>{ x, wholePart, z }).Sum()];

				node.myDirt = fraction;
				node.myRock = std::numeric_limits<decltype(node.myDirt)>::min();
				node.myIron = std::numeric_limits<decltype(node.myDirt)>::min();
				node.myGold = std::numeric_limits<decltype(node.myDirt)>::min();
			}

			for (size_t y = wholePart + 1; y < myResolution.y; y++)
			{
				TerrainNode& node = myTerrainData[(CommonUtilities::Vector3<size_t>{1, myResolution.x, myResolution.x * myResolution.y } * CommonUtilities::Vector3<size_t>{ x, y, z }).Sum()];

				node.myDirt = std::numeric_limits<decltype(node.myDirt)>::min();
				node.myRock = std::numeric_limits<decltype(node.myDirt)>::min();
				node.myIron = std::numeric_limits<decltype(node.myDirt)>::min();
				node.myGold = std::numeric_limits<decltype(node.myDirt)>::min();
			}
		}
	}
	
	for (size_t x = 0; x < myResolution.x; x++)
	{
		for (size_t y = 0; y < myResolution.y; y++)
		{
			for (size_t z = 0; z < myResolution.z; z++)
			{
				if (x == 0 || x == myResolution.x - 1 || y == 0 || y == myResolution.y - 1 || z == 0 || z == myResolution.z - 1)
				{
					myTerrainData[(CommonUtilities::Vector3<size_t>{1, myResolution.x, myResolution.x* myResolution.y } *CommonUtilities::Vector3<size_t>{ x, y, z }).Sum()] = TerrainNode{};
				}
			}
		}
	}
	

	myIsDirty = true;
}

void Terrain::FromFile(const std::string& aFilePath)
{
	std::string ext = Tools::ExtensionFromPath(aFilePath);

	if (ext == ".ter")
	{
		FromTER(aFilePath);
		return;
	}
	else
	{
		SYSERROR("Terrain does not know how to load files of this type", aFilePath);
	}
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

	if (offset < fileContent.size())
	{
		SYSWARNING("Unconsumed data in terrain file", aFilePath, Tools::PrintByteSize(fileContent.size() - offset));
	}
}


void Terrain::Setup()
{
	myIsDirty = true;

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
	myModelData->myIsIndexed = false;

	model->AddModelPart(myModelData);
	myModelAsset = new ModelAsset(model, "");
	myModelHandle = AssetHandle(myModelAsset);

	model->MarkLoaded();

	myModelInstance = myModelHandle.InstansiateModel();

	RenderScene::GetInstance().AddToScene(myModelInstance);
}

void Terrain::SetupGraphicsResources()
{
	ID3D11Device* device = myFramework->GetDevice();

	ID3D11Buffer* vertexBuffer;

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

	Tools::ExecuteOnDestruct releaseVertexBuffer([vertexBuffer]() { vertexBuffer->Release(); });

	D3D11_INPUT_ELEMENT_DESC layout[3] =
	{
		{ "SV_POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",				0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "RANDOM_SEED",		0, DXGI_FORMAT_R32_UINT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ID3D11InputLayout* inputLayout;
	result = device->CreateInputLayout(layout, 3, myModelData->myVertexShader.GetVertexShaderblob().data(), myModelData->myVertexShader.GetVertexShaderblob().size(), &inputLayout);
	if (FAILED(result))
	{
		SYSERROR("Could not create inputlayout", "Terrain");
		return;
	}
	Tools::ExecuteOnDestruct releaseInputLayout([inputLayout]() { inputLayout->Release(); });


	myModelData->myVertexBuffer = vertexBuffer;
	myModelData->myInputLayout = inputLayout;

	releaseVertexBuffer.Disable();
	releaseInputLayout.Disable();
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

Terrain::TerrainNode::operator float()
{
	if (Empty()) { return 0.f; }

	float sum = static_cast<float>(myDirt + myRock + myIron + myGold)/256.f;
	return 1.f + 100.f / (std::pow(2.f, (1.f - sum) * 14.f));
}

void Terrain::TerrainNode::AttemptDig(DigResult& aInOutResult, size_t aPower)
{
	for (size_t i = 0; i < aPower && !Empty(); i++)
	{
		if (myDirt > 0) { aInOutResult.myDirt++; myDirt--; }
		if (myRock > 0) { aInOutResult.myRock++; myRock--; }
		if (myIron > 0) { aInOutResult.myIron++; myIron--; }
		if (myGold > 0) { aInOutResult.myGold++; myGold--; }
	}
}
