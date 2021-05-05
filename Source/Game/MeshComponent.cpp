#include "pch.pch"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Scene.h"
#include "Entity.h"

Mesh::Mesh() :
	myModelLoader(nullptr),
	myScene(nullptr),
	myModelInstance(nullptr)
{
	myComponentType = ComponentType::Mesh;
}

Mesh::~Mesh()
{
	
}

void Mesh::PreInit(CModelLoader* aModelLoader, CScene* aScene)
{
	myModelLoader = aModelLoader;
	myScene = aScene;
}

void Mesh::Init(Entity* aEntity)
{
	myEntity = aEntity;	
}

void Mesh::SetUpModel(std::string aPath)
{
	myModelInstance = myModelLoader->InstantiateModel(aPath);
	myScene->AddModel(myModelInstance);
}

void Mesh::Update(const float aDeltaTime)
{
	//render command? probably no, scene will render
	CommonUtilities::Vector3<float> bob = myEntity->GetPosition();
	GetModelInstance()->SetPosition(CommonUtilities::Vector4<float>(bob.x, bob.y, bob.z, 0.0f));
}

void Mesh::Reset()
{
	myScene->RemoveModel(myModelInstance);
	delete myModelInstance;
	myModelInstance = nullptr;
}

CModelInstance* Mesh::GetModelInstance()
{
	return myModelInstance;
}
