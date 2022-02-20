#include "pch.h"
#include "MeshComponent.h"
#include "RenderScene.h"
#include "GamlaBettan\AssetManager.h"
#include "GamlaBettan\ModelInstance.h"
#include "GamlaBettan\Entity.h"
#include "GamlaBettan\EntityManager.h"


MeshComponent::MeshComponent(const UseDefaults&) : MeshComponent("engine/defaultCube.fbx")
{
}

MeshComponent::MeshComponent(const FiskJSON::Object& aObject)
{
	std::string filePath;
	if (!aObject["modelPath"].GetIf(filePath))
	{
		myInstance = AssetManager::GetInstance().GetModel("engine/defaultCube.fbx").InstansiateModel();
		RenderScene::GetInstance().AddToScene(myInstance);
		return;
	}
	myInstance = AssetManager::GetInstance().GetModel(filePath).InstansiateModel();
	RenderScene::GetInstance().AddToScene(myInstance);
}

MeshComponent::MeshComponent(const std::string& aModelname) : MeshComponent(AssetManager::GetInstance().GetModel(aModelname))
{
}

MeshComponent::MeshComponent(AssetHandle aModelAsset)
{
	myInstance = aModelAsset.InstansiateModel();
	RenderScene::GetInstance().AddToScene(myInstance);
}

MeshComponent::~MeshComponent()
{
	RenderScene::GetInstance().RemoveFromScene(myInstance);
	delete myInstance;
}

void MeshComponent::Update(const FrameData& /*aFrameData*/, EntityID aEntityID)
{
	myInstance->SetTransform(EntityManager::GetInstance().Retrieve(aEntityID)->myTransform);
}

void MeshComponent::ImGui(EntityID /*aEntityID*/)
{
	myInstance->SetIsHighlighted(ImGui::IsWindowHovered());
	ImGui::Button("Drop Zone",ImVec2(100,50));
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset");
		if (payload)
		{
			AssetHandle asset = *static_cast<Asset**>(payload->Data);
			if (asset.GetType() == Asset::AssetType::Model)
			{
				RenderScene::GetInstance().RemoveFromScene(myInstance);
				delete myInstance;

				myInstance = asset.InstansiateModel();
				RenderScene::GetInstance().AddToScene(myInstance);
			}
			else
			{
				SYSWARNING("asset dropped on meshComponent is not a model");
			}
		}

		ImGui::EndDragDropTarget();
	}
}

void MeshComponent::Serialize(FiskJSON::Object& aObject)
{
	aObject.AddValueChild("modelPath", myInstance->GetModelAsset().GetModelPath());
}
