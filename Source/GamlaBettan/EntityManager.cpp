#include "pch.h"
#include "EntityManager.h"
#include "GamlaBettan\MeshComponent.h"

#define SCENE_BASE_FOLDER "../baked/scenes/"
#define SCENE_EXTENSION ".scene"

EntityID EntityManager::MakeEntity()
{
    if (myUnused.empty())
    {
        myUnused.reserve(myBatchSize);
        std::array<Entity, myBatchSize>* nextBuffer = new std::array<Entity, myBatchSize>();
        for (int i = myBatchSize-1; i >= 0; i--)
        {
            myUnused.push_back(static_cast<EntityID>(myPools.size() * myBatchSize + i + myIDStartOffset));
            nextBuffer->at(i).myID = static_cast<EntityID>(myPools.size() * myBatchSize + i + myIDStartOffset);
        }
        myPools.push_back(nextBuffer);
    }

    EntityID out = myUnused.back();
    myUnused.pop_back();

    Retrieve(out)->Activate();
    return out;
}

Entity* EntityManager::Retrieve(EntityID aID)
{
    return &(myPools[(aID - myIDStartOffset) / myBatchSize]->at((aID-myIDStartOffset) % myBatchSize));
}

void EntityManager::Return(EntityID& aID)
{
    Retrieve(aID)->Deactivate();
    myUnused.push_back(aID);
}

#if USEIMGUI
void EntityManager::ImGui()
{
    WindowControl::Window("Entity Manager", [this]() {

        static char FilePathBuffer[MAX_PATH + 1] = "defaultScene";
        ImGui::InputText("File", FilePathBuffer, MAX_PATH + 1);
        if (ImGui::Button("Save"))
        {
            Scene scene(GetAllActive());
            SaveScene(scene, SCENE_BASE_FOLDER + std::string(FilePathBuffer) + SCENE_EXTENSION);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            auto _ = LoadScene(SCENE_BASE_FOLDER + std::string(FilePathBuffer) + SCENE_EXTENSION);
        }

        ImGui::Text("Current Unused %d", myUnused.size());

        ComponentManager::GetInstance().ImGui();

        for (int poolIndex = 0; poolIndex < myPools.size(); poolIndex++)
        {
            ImGui::PushID(poolIndex);
            if (ImGui::TreeNode("node", "%d", poolIndex))
            {
                int unusedCounter = 0;
                for (int i = 0; i < myBatchSize; i++)
                {
                    Entity& entity = myPools[poolIndex]->at(i);
                    EntityID id = entity.myID;
                    if (std::find(myUnused.begin(),myUnused.end(),id) != myUnused.end())
                    {
                        unusedCounter++;
                        continue;
                    }
                    if (unusedCounter > 0)
                    {
                        ImGui::Text("Unused Entity x%d",unusedCounter);
                        unusedCounter = 0;
                    }

                    ImGui::PushID(i);
                    bool open = ImGui::TreeNode(&entity, entity.myEditorName.c_str());

                    if (ImGui::BeginDragDropTarget())
                    {
                        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ComponentAdd");
                        if (payload)
                        {
                            (*reinterpret_cast<ComponentSystemBase**>(payload->Data))->AddDefaultComponent(id);
                        }
                        payload = ImGui::AcceptDragDropPayload("Asset");
                        if (payload)
                        {
                            AssetHandle asset = *reinterpret_cast<Asset**>(payload->Data);
                            switch (asset.GetType())
                            {
                            case Asset::AssetType::Model:
                                entity.AddComponent<MeshComponent>(asset);
                                break;
                            default:
                                SYSWARNING("Dont know what to do assettype",std::to_string(static_cast<int>(asset.GetType())));
                                break;
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    if(open)
                    {
                        if (ImGui::BeginChild("window", ImVec2(0,0), true))
                        {
                            Tools::EditPosition("Position", &entity.myTransform[4 * 3]);
                            ComponentManager::GetInstance().RunImGuiFor(id);
                            ImGui::InvisibleButton("drop target", ImGui::GetContentRegionAvail());

                            if (ImGui::BeginDragDropTarget())
                            {
                                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ComponentAdd");
                                if (payload)
                                {
                                    (*reinterpret_cast<ComponentSystemBase**>(payload->Data))->AddDefaultComponent(id);
                                }
                                payload = ImGui::AcceptDragDropPayload("Asset");
                                if (payload)
                                {
                                    AssetHandle asset = *reinterpret_cast<Asset**>(payload->Data);
                                    switch (asset.GetType())
                                    {
                                    case Asset::AssetType::Model:
                                        entity.AddComponent<MeshComponent>(asset);
                                        break;
                                    default:
                                        SYSWARNING("Dont know what to do assettype", std::to_string(static_cast<int>(asset.GetType())));
                                        break;
                                    }
                                }

                                ImGui::EndDragDropTarget();
                            }
                        }
                        ImGui::EndChild();
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                if (unusedCounter > 0)
                {
                    ImGui::Text("Unused Entity x%d", unusedCounter);
                    unusedCounter = 0;
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        });

}

std::vector<EntityID> EntityManager::GetAllActive()
{
	std::vector<EntityID> out;
	for (EntityID poolIndex = 0; poolIndex < myPools.size(); poolIndex++)
	{
		for (EntityID i = 0; i < myBatchSize; i++)
		{
			if ((*myPools[poolIndex])[i].myIsActive)
			{
				out.push_back(myIDStartOffset + poolIndex * myBatchSize + i);
			}
		}
	}
    return out;
}

void EntityManager::SaveScene(Scene& aScene, const std::string& aFilePath)
{
	FiskJSON::Object root;
	FiskJSON::Object* entities = new FiskJSON::Object();
	entities->MakeArray();
    SaveCollection(aScene.myEntities, entities->Get<FiskJSON::ArrayWrapper>());
	root.AddValueChild("version", 1);
	root.AddChild("entities", entities);

	std::filesystem::create_directories(Tools::PathWithoutFile(aFilePath));
	std::ofstream outFile(aFilePath);
	outFile << root.Serialize();
	if (!outFile)
	{
		SYSERROR("Failed to save scene to file", aFilePath);
	}
}

void EntityManager::SaveCollection(const std::vector<EntityID>& aEntities, FiskJSON::ArrayWrapper aObject)
{
	for (auto& id : aEntities)
	{
		Entity& entity = *Retrieve(id);
		FiskJSON::Object* entityObject = new FiskJSON::Object();
		entity.Serialize(*entityObject);
        aObject.PushChild(entityObject);
	}
}

Scene EntityManager::LoadScene(const std::string& aFilePath)
{
	FiskJSON::Object root;
	try
	{
		root.Parse(Tools::ReadWholeFile(aFilePath));
	}
	catch (const std::exception& e)
	{
		SYSERROR("Failed to load scene, json failed to parse", aFilePath, e.what());
		return {};
	}

	int version;
	if (!root["version"].GetIf(version))
	{
		SYSERROR("Failed to load scene, no version number", aFilePath);
		return {};
	}
    return Scene(LoadCollection(root));
}

std::vector<EntityID> EntityManager::LoadCollection(const FiskJSON::Object& aObject)
{
	std::vector<EntityID> out;

	for (FiskJSON::Object& entity : aObject["entities"].Get<FiskJSON::ConstArrayWrapper>())
	{
        EntityID id = MakeEntity();
        ComponentManager::GetInstance().Deserialize(id, entity);
        out.push_back(id);
	}

    return out;
}
#endif
