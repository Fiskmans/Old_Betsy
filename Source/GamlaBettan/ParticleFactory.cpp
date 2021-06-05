#include "pch.h"
#include "ParticleFactory.h"
#include "DirectX11Framework.h"
#include "ParticleInstance.h"
#include "TextureLoader.h"
#include <d3d11.h>
#include "ShaderCompiler.h"
#include <fstream>
#include "WindSystem.h"
#include "AssetManager.h"

#if USEIMGUI
#include "Scene.h"
#include "Camera.h"
#endif

#pragma region ParticleSaveVersionHandling
namespace SavingAndLoading
{
	void LoadInto(Particle::Data::Customizable& aParticle,const std::string& aFilepath);
	void SaveFrom(Particle::Data::Customizable& aParticle,const std::string& aFilepath);
	void Default(Particle::Data::Customizable& aParticle);
	void Reconstruct(ID3D11Device* aDevice, Particle& aParticle);
}
#pragma endregion

#pragma region Constructing
namespace Constructing
{
	ID3D11Buffer* CreateBufferFor(ID3D11Device* aDevice, Particle::Data& aParticle);
}
#pragma endregion


struct ParticalEditData
{
	std::unordered_map<std::string, Particle*>::iterator editing;
	ParticleInstance* instance;
	float min[3] = { 0,0,0 };
	float max[3] = { 1,1,1 };
};


ParticleFactory::ParticleFactory() :
	myDevice(nullptr),
	myInputLayout(nullptr)
{
}
ParticleFactory::~ParticleFactory()
{
	for (auto& i : myParticles)
	{
		SAFE_DELETE(i.second);
	}
}

bool ParticleFactory::Init(DirectX11Framework* aFramework)
{
	myDevice = aFramework->GetDevice();

	myPixelShader = AssetManager::GetInstance().GetPixelShader("Particle.hlsl");
	myGeometryShader = AssetManager::GetInstance().GetGeometryShader("Particle.hlsl");
	myVertexShader = AssetManager::GetInstance().GetVertexShader("Particle.hlsl");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "MOVEMENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DISTANCE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVMIN", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "UVMAX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "FBTIMER", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT result = myDevice->CreateInputLayout(layout, sizeof(layout) / sizeof(layout[0]), myVertexShader.GetVertexShaderblob().data(), myVertexShader.GetVertexShaderblob().size(), &myInputLayout);
	if (FAILED(result))
	{
		SYSERROR("could not create input layout for particle");
		return false;
	}

	static bool once = true;
	if (once)
	{
		WindSystem::GetInstance().Init(V3F(2.f, 0.f, 3.f).GetNormalized());
		once = false;
	}

	return true;
}

ParticleInstance* ParticleFactory::InstantiateParticle(const std::string& aFilePath)
{
	if (!myDevice)
	{
		SYSERROR("Yo wtf, init pls");
	}

	Particle* baseParticle = nullptr;
	if (myParticles.count(aFilePath) == 0)
	{
		baseParticle = LoadParticle(aFilePath);
		myParticles[aFilePath] = baseParticle;
	}
	else
	{
		baseParticle = myParticles[aFilePath];
	}
	ParticleInstance* instance = new ParticleInstance();
	instance->SetDirection(V4F(0, 1, 0, 0));
	instance->Init(baseParticle);
	return instance;
}

#if USEIMGUI
void EditOverTime(Particle::Data::Customizable::OverTime& aOverTime)
{
	ImGui::DragFloat("Size", &aOverTime.mySize,0.01f);
	ImGui::ColorPicker4("Color", &aOverTime.myParticleColor.x);
}

void ParticleFactory::EditParticles(Scene* aScene)
{
	static V4F direction;
	if (ImGui::Button("Check For Particles"))
	{
		std::experimental::filesystem::directory_iterator it(std::experimental::filesystem::canonical("Data/Particles/"));
		while (it != std::experimental::filesystem::directory_iterator())
		{
			if (it->path().has_extension())
			{
				if (it->path().extension() == ".part")
				{
					delete InstantiateParticle(it->path().filename().string());
				}
			}
			it++;
		}
	}

	static int numberToEdit = 1;
	ImGui::InputInt("Number of Particles to edit", &numberToEdit);

	static std::vector<ParticalEditData> myParticalsEditing;

	if (numberToEdit > myParticalsEditing.size())
	{
		for (int i = myParticalsEditing.size(); i < numberToEdit; ++i)
		{
			if (i == myParticalsEditing.size())
			{
				ParticalEditData data;
				data.instance = nullptr;
				data.editing = myParticles.end();
				myParticalsEditing.push_back(data);
			}
		}
	}
	else if (numberToEdit < myParticalsEditing.size())
	{
		for (int i = myParticalsEditing.size() - 1; i >= numberToEdit; --i)
		{
			if (myParticalsEditing[i].instance)
			{
				delete myParticalsEditing[i].instance;
				delete myParticalsEditing[i].editing->second;
			}
			myParticalsEditing.erase(myParticalsEditing.begin() + i);
		}
	}


	bool directionChanged = false;
	if (ImGui::DragFloat3("Direction", &direction.x, 0.01f, -2, 2))
	{
		directionChanged = true;
	}

	for (auto particle : myParticalsEditing)
	{
		if (!aScene->Contains(particle.instance))
		{
			particle.instance = nullptr;
		}
		if (particle.instance)
		{
			particle.instance->RefreshTimeout(1.f);
		}
		if (directionChanged)
		{
			if (particle.instance)
			{
				particle.instance->SetDirection(direction);
			}
		}
	}
	ImGui::Separator();
	auto it = myParticles.begin();
	while(it != myParticles.end())
	{
		if (ImGui::Selectable(it->first.c_str()))
		{
			bool shouldClear = true;
			for (auto& particle : myParticalsEditing)
			{
				if (particle.instance == nullptr)
				{
					particle.editing = it;
					particle.instance = InstantiateParticle(it->first);
					particle.instance->SetDirection(direction);
					aScene->AddInstance(particle.instance);

					shouldClear = false;
					break;
				}
			}

			if (shouldClear)
			{
				for (auto& particle : myParticalsEditing)
				{
					if (particle.instance)
					{
						particle.editing = myParticles.end();
						aScene->RemoveFrom(particle.instance);
						SAFE_DELETE(particle.instance);
					}
				}

				for (auto& particle : myParticalsEditing)
				{
					if (particle.instance == nullptr)
					{
						particle.editing = it;
						particle.instance = InstantiateParticle(it->first);
						particle.instance->SetDirection(direction);
						aScene->AddInstance(particle.instance);
						break;
					}
				}
			}
		}
		it++;
	}

	//Jonas Kanske fixar detta nån gång.
	//I alla fall är det ett försök att spara ner stuff till fil så man kan sätta på flera particlar på en entitet från partikeleditorn

	//static char type[16];
	//ImGui::InputText("Emitter Type", type, 16);

	//if (ImGui::Button("Save EmitterData"))
	//{
	//	std::string fileContent = Tools::ReadWholeFile("Data/Particles/EmitterData.txt");

	//	size_t pos = fileContent.find("\n");
	//	std::string line = fileContent.substr(0, pos);
	//	size_t pos2 = fileContent.find("\n", pos + 1);


	//	while (pos2 < fileContent.size())
	//	{
	//		line = fileContent.substr(pos + 1, pos2 - (pos + 1));


	//		if ()
	//		{

	//			fileContent.replace(pos + 1, pos2 - (pos + 1), "Tjena");
	//		}
	//		
	//		pos = pos2;
	//		pos2 = fileContent.find("\n", pos + 1);
	//	}
	//	
	//	//bool typeExcist = false;

	//	//while (std::getline(, line))
	//	//{
	//	//	std::stringstream ss(line);
	//	//	std::string word;
	//	//	ss >> word;

	//	//	if (word == type)
	//	//	{
	//	//		typeExcist = true;

	//	//		std::getline(file, line);
	//	//		for (auto& particle : myParticalsEditing)
	//	//		{
	//	//			file << particle.editing->first.c_str() << ": ";
	//	//			file << "Min: " << particle.min[0] << " " << particle.min[1] << " " << particle.min[2] << ", ";
	//	//			file << "Max: " << particle.max[0] << " " << particle.max[1] << " " << particle.max[2] << ", ";
	//	//			file << "\n";
	//	//		}
	//	//	}
	//	//}

	//	//if (!typeExcist)
	//	//{
	//	//	//Write
	//	//	file << type << "\n";
	//	//	for (auto& particle : myParticalsEditing)
	//	//	{
	//	//		file << particle.editing->first.c_str() << ": ";
	//	//		file << "Min: " << particle.min[0] << " " << particle.min[1] << " " << particle.min[2] << ", ";
	//	//		file << "Max: " <<  particle.max[0] << " " << particle.max[1] << " " << particle.max[2] << ", ";
	//	//		file << "\n";
	//	//	}
	//	//}

	//	//file.close();
	//}

	for (int i = 0; i < myParticalsEditing.size(); ++i)
	{
		ImGui::Separator();
		ImGui::PushID(i);

		std::unordered_map<std::string, Particle*>::iterator editing = myParticalsEditing[i].editing;
		ParticleInstance* instance = myParticalsEditing[i].instance;


		if (editing != myParticles.end())
		{
			ImGui::Text(editing->first.c_str());
			Particle::Data::Customizable& customizable = editing->second->GetData().myCustomizable;

			if (ImGui::Button("Save"))
			{
				SavingAndLoading::SaveFrom(customizable, "Data/Particles/" + editing->first);


			}
			ImGui::SameLine();
			if (ImGui::Button("Load"))
			{
				SavingAndLoading::LoadInto(customizable, "Data/Particles/" + editing->first);

				editing->second->GetData().myTexture = AssetManager::GetInstance().GetTexture(customizable.myFilePath);
			}
			{
				std::string before = customizable.myFilePath;
				ImGui::InputText("Texture", customizable.myFilePath, 96);


				if (before != customizable.myFilePath)
				{
					editing->second->GetData().myTexture = AssetManager::GetInstance().GetTexture(customizable.myFilePath);
				}
			}


			V4F emitterMinPos;
			V4F emitterMaxPos;

			ImGui::InputFloat3("Min", myParticalsEditing[i].min);
			ImGui::InputFloat3("Max", myParticalsEditing[i].max);

			emitterMinPos.x = myParticalsEditing[i].min[0];
			emitterMinPos.y =  myParticalsEditing[i].min[1];
			emitterMinPos.z =  myParticalsEditing[i].min[2];
			emitterMinPos.w = 1;

			emitterMaxPos.x =  myParticalsEditing[i].max[0];
			emitterMaxPos.y =  myParticalsEditing[i].max[1];
			emitterMaxPos.z =  myParticalsEditing[i].max[2];
			emitterMaxPos.w = 1;

			if (emitterMinPos.LengthSqr() < emitterMaxPos.LengthSqr())
			{
				instance->SetBounds(emitterMinPos, emitterMaxPos);
			}
				
			float before = customizable.mySpawnRate;
			if (ImGui::DragFloat("Emission Rate",&customizable.mySpawnRate,0.1f,0.001f,10000.f))
			{
				if (before > customizable.myParticleLifetime)
				{
					SavingAndLoading::Reconstruct(myDevice, *editing->second);
				}
			}
			before = customizable.myParticleLifetime;
			if (ImGui::DragFloat("Lifetime", &customizable.myParticleLifetime,0.1f,0.01f,1000.f))
			{
				if (before < customizable.myParticleLifetime)
				{
					SavingAndLoading::Reconstruct(myDevice, *editing->second);
				}
			}
			ImGui::DragFloat("Seperation", &customizable.mySeperation, 0.01f, 0.0f, 3.0f);
			ImGui::SameLine();
			if (ImGui::Button("Default"))
			{
				customizable.mySeperation = 1.f;
			}
			ImGui::DragFloat3("Gravity xyz", &customizable.myGravity.x,0.1f, -20.f, 20.f);
			ImGui::SameLine();
			if (ImGui::Button("Zero"))
			{
				customizable.myGravity = V3F(0, 0, 0);
			}

			ImGui::DragFloat("Speed", &customizable.myParticleSpeed);
			ImGui::DragFloat("Drag", &customizable.myDrag,0.01f,0.0f,100.0f);

			ImGui::DragFloat("Time Per Frame", &customizable.myFlipBook.myTime);
			ImGui::DragInt("Pages", &customizable.myFlipBook.myPages);
			ImGui::DragInt("SizeX", &customizable.myFlipBook.mySizeX);
			ImGui::DragInt("SizeY", &customizable.myFlipBook.mySizeY);

			ImGui::Columns(2);
			ImGui::Text("Start");
			ImGui::PushID("Start");
			EditOverTime(customizable.myStart);
			ImGui::PopID();
			ImGui::NextColumn();
			ImGui::Text("End");
			ImGui::PushID("End");
			EditOverTime(customizable.myEnd);
			ImGui::PopID();
			ImGui::NextColumn();
		}

		ImGui::PopID();
	}

}
#endif

Particle* ParticleFactory::LoadParticle(const std::string& aFilePath)
{

	Particle::Data data;


	SavingAndLoading::LoadInto(data.myCustomizable, "Data/Particles/" + aFilePath);


	data.myOffset = 0;
	data.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	data.myInputLayout = myInputLayout;
	data.myVertexShader = myVertexShader;
	data.myGeometryShader = myGeometryShader;
	data.myPixelShader = myPixelShader;
	data.myStride = sizeof(Particle::Vertex);

	std::string path = data.myCustomizable.myFilePath;
	data.myTexture = AssetManager::GetInstance().GetTexture(path);

	data.myParticleVertexBuffer = Constructing::CreateBufferFor(myDevice, data);

	Particle* particle = new Particle();
	particle->Init(data);
	return particle;
}



namespace SavingAndLoading
{
	const static unsigned short CurrentVersion = 2;
	struct Header
	{
		unsigned short myVersionNumber;
		char unused[30];
	};

	namespace Version0
	{
		void LoadInto(LegacyParticleTypes::Customizable_V1& aParticle, Header& aHeader,std::ifstream& aFileStream)
		{
			LegacyParticleTypes::Customizable_V0 buffer;
			aFileStream.read(reinterpret_cast<char*>(&buffer), sizeof(LegacyParticleTypes::Customizable_V0));
			aParticle.myEnd.myParticleColor = buffer.myEnd.myParticleColor;
			aParticle.myEnd.mySize = buffer.myEnd.mySize;
			memcpy(aParticle.myFilePath,buffer.myFilePath, sizeof(buffer.myFilePath));
			aParticle.myParticleLifetime = buffer.myParticleLifetime;
			aParticle.myParticleSpeed = buffer.myParticleSpeed;

			aParticle.mySeperation = buffer.mySpawnAgnle;
			aParticle.mySpawnRate = buffer.mySpawnRate;
			aParticle.myStart.myParticleColor = buffer.myStart.myParticleColor;
			aParticle.myStart.mySize = buffer.myStart.mySize;
			aParticle.myGravity = V3F(0,0,0);
			aParticle.myDrag = 0.0f;
		}
	}

	namespace Version1
	{
		void LoadInto(Particle::Data::Customizable& aParticle, Header& aHeader, std::ifstream& aFileStream)
		{
			LegacyParticleTypes::Customizable_V1 buffer;
			if (aHeader.myVersionNumber < 1)
			{
				Version0::LoadInto(buffer, aHeader, aFileStream);
			}
			else
			{
				aFileStream.read(reinterpret_cast<char*>(&buffer), sizeof(LegacyParticleTypes::Customizable_V1));
			}

			aParticle.myEnd.myParticleColor = buffer.myEnd.myParticleColor;
			aParticle.myEnd.mySize = buffer.myEnd.mySize;
			memcpy(aParticle.myFilePath, buffer.myFilePath, sizeof(buffer.myFilePath));
			aParticle.myParticleLifetime = buffer.myParticleLifetime;
			aParticle.myParticleSpeed = buffer.myParticleSpeed;

			aParticle.mySeperation = buffer.mySeperation;
			aParticle.mySpawnRate = buffer.mySpawnRate;
			aParticle.myStart.myParticleColor = buffer.myStart.myParticleColor;
			aParticle.myStart.mySize = buffer.myStart.mySize;
			aParticle.myGravity = V3F(0, 0, 0);
			aParticle.myDrag = 0.0f;

			aParticle.myFlipBook.myTime = 0;
			aParticle.myFlipBook.myPages = 0;
			aParticle.myFlipBook.mySizeX = 0;
			aParticle.myFlipBook.mySizeY = 0;
		}
	}
	void LoadInto(Particle::Data::Customizable& aParticle, Header& aHeader, std::ifstream& aFileStream)
	{
		aFileStream.read(reinterpret_cast<char*>(&aParticle), sizeof(Particle::Data::Customizable));
	}


	void LoadInto(Particle::Data::Customizable& aParticle, const std::string& aFilepath)
	{
		static_assert(CHAR_BIT == 8, "Assumtion wrong");
		static_assert(sizeof(Header) == 32, "Header struct is invalid");
		Header header;
		std::ifstream infile;
		infile.open(aFilepath,std::ios::binary | std::ios::in);			//Open file
		infile.read(reinterpret_cast<char*>(&header), sizeof(Header));	//Read header
		if (infile)														//Load using appropriate loader
		{
			if (header.myVersionNumber == CurrentVersion)
			{
				LoadInto(aParticle, header, infile);
			}
			else
			{
				if (header.myVersionNumber > CurrentVersion)
				{
					SYSERROR("Particle is not compatible with this version", aFilepath);
					Default(aParticle);
				}
				else
				{
					LOGWARNING("Particle is outdated and will be loaded using a legacy loader. file version: " + std::to_string(header.myVersionNumber) + " code version: " + std::to_string(CurrentVersion), aFilepath);
					switch (header.myVersionNumber)
					{
					case 0:
					case 1:
						Version1::LoadInto(aParticle, header, infile);
						break;

					default:
						SYSERROR("Trying to load particle with a unsupported version", aFilepath);
						break;
					}
				}
			}
		}
		else
		{
			Default(aParticle);
		}
	}
	void SaveFrom(Particle::Data::Customizable& aParticle, const std::string& aFilepath)
	{
		static_assert(CHAR_BIT == 8, "Assumtion wrong");
		static_assert(sizeof(Header) == 32, "Header struct is invalid");
		
		Header header;
		header.myVersionNumber = CurrentVersion;

		std::ofstream outfile;
		outfile.open(aFilepath, std::ios::out | std::ios::binary);
		outfile.write(reinterpret_cast<char*>(&header), sizeof(Header));
		outfile.write(reinterpret_cast<char*>(&aParticle), sizeof(Particle::Data::Customizable));
		

	}
	void Reconstruct(ID3D11Device* aDevice, Particle& aParticle)
	{
		ID3D11Buffer* buffer = Constructing::CreateBufferFor(aDevice,aParticle.GetData());

		if (buffer)
		{
			aParticle.GetData().myParticleVertexBuffer->Release();
			aParticle.GetData().myParticleVertexBuffer = buffer;
		}
	}
	void Default(Particle::Data::Customizable& aParticle)
	{
		aParticle.mySeperation = 1;
		aParticle.myParticleSpeed = 100;
		aParticle.myParticleLifetime = 3.5f;
		aParticle.mySpawnRate = 40;

		aParticle.myStart.myParticleColor = { 1,1,0,0.8f };
		aParticle.myStart.mySize = 10;

		aParticle.myEnd.myParticleColor = { 1,0,0,0 };
		aParticle.myEnd.mySize = 5;

		
		strcpy_s(aParticle.myFilePath,96, "Data/Textures/gamlaBettan_Alpha.dds");
	}
}

namespace Constructing
{
	ID3D11Buffer* CreateBufferFor(ID3D11Device* aDevice, Particle::Data& aParticle)
	{
		CD3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = CAST(UINT, sizeof(Particle::Vertex) * aParticle.myCustomizable.myParticleLifetime * aParticle.myCustomizable.mySpawnRate);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		ID3D11Buffer* buffer;

		DirectX11Framework::AddGraphicsMemoryUsage(desc.ByteWidth, "Unkown Particle", "Particles");
		HRESULT result = aDevice->CreateBuffer(&desc, nullptr, &buffer);
		if (FAILED(result))
		{
			SYSERROR("Could not create vertex buffer for particles");
			return nullptr;
		}
		return buffer;
	}
}