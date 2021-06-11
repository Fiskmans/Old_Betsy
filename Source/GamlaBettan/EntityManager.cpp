#include "pch.h"
#include "EntityManager.h"
#include "GamlaBettan\MeshComponent.h"

EntityID EntityManager::Get()
{
    if (myUnused.empty())
    {
        myUnused.reserve(myBatchSize);
        std::array<Entity, myBatchSize>* nextBuffer = new std::array<Entity, myBatchSize>();
        for (int i = myBatchSize-1; i >= 0; i--)
        {
            myUnused.push_back(myPools.size() * myBatchSize + i + myIDStartOffset);
            nextBuffer->at(i).myID = myPools.size() * myBatchSize + i + myIDStartOffset;
        }
        myPools.push_back(nextBuffer);
    }

    EntityID out = myUnused.back();
    myUnused.pop_back();

    Retrieve(out)->Construct();
    return out;
}

Entity* EntityManager::Retrieve(EntityID aID)
{
    return &(myPools[(aID - myIDStartOffset) / myBatchSize]->at((aID-myIDStartOffset) % myBatchSize));
}

void EntityManager::Return(EntityID& aID)
{
    Retrieve(aID)->Destruct();
    myUnused.push_back(aID);
}

#if USEIMGUI
void EntityManager::ImGui()
{
    WindowControl::Window("Entity Manager", [this]() {

        ImGui::Text("Current Unused %d", myUnused.size());

        ComponentManager::GetInstance().ImGui();

        for (size_t poolIndex = 0; poolIndex < myPools.size(); poolIndex++)
        {
            ImGui::PushID(poolIndex);
            if (ImGui::TreeNode("node", "%d", poolIndex))
            {
                int unusedCounter = 0;
                for (size_t i = 0; i < myBatchSize; i++)
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
#endif
