#include "pch.h"
#include "MeshComponent.h"
#include "Scene.h"
#include "GamlaBettan\AssetManager.h"
#include "GamlaBettan\ModelInstance.h"
#include "GamlaBettan\Entity.h"


MeshComponent::MeshComponent(const std::string& aModelname)
{
	myInstance = AssetManager::GetInstance().GetModel(aModelname).InstansiateModel();
	Scene::GetInstance().AddToScene(myInstance);
}

MeshComponent::~MeshComponent()
{
	Scene::GetInstance().RemoveFromScene(myInstance);
}

void MeshComponent::Update(const FrameData& aFrameData, Entity* aEntity)
{
	myInstance->SetTransform(aEntity->myTransform);
}
