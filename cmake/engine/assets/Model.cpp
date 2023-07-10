
#include "engine/assets/Model.h"

namespace engine
{
	Model::Model() : myModelData()
	{
	}

	Model::~Model()
	{
	}

	void Model::AddModelPart(ModelData* aPart)
	{
		myModelData.push_back(aPart);
	}

	const std::vector<Model::ModelData*> Model::GetModelData() const
	{
		return myModelData;
	}

	/*
	void Model::ImGuiNode(std::map<std::string, std::vector<std::string>>& aFiles)
	{
		ImGui::Text("Renders: ");
		ImGui::SameLine();
		ImGui::PushID("should render");
		ImGui::Checkbox("", &myShouldRender);
		ImGui::PopID();

		for (ModelData* data : myModelData)
		{
			ImGui::PushID(data);
			size_t flags = data->myshaderTypeFlags;
			if (flags & ShaderFlags::HasVertexColors)
			{
				ImGui::Text("Has VertexColors");
			}
			if (flags & ShaderFlags::HasUvSets)
			{
				ImGui::Text("Has Uv sets");
			}
			if (flags & ShaderFlags::HasBones)
			{
				ImGui::Text("Has bones");
			}
			if (data->myUseForwardRenderer)
			{
				ImGui::Text("Draws using forward renderer");
			}
			float width = ImGui::GetWindowSize().x / 3;
			ImVec2 imagesize = ImVec2(width, width);
			ImGui::Columns(3);
			static const char* imageNames[3] = { "Albedo:","Normal:","Material:" };
			for (size_t i = 0; i < 3; i++)
			{
				ImGui::PushID(imageNames[i]);
				ImGui::Text(imageNames[i]);

				Tools::ZoomableImGuiImage(data->myTextures[i].GetAsTexture(), imagesize);


				if (ImGui::Button("Change"))
				{
					ImGui::OpenPopup("ImageSelector");
				}
				if (ImGui::BeginPopup("ImageSelector"))
				{
					ImGui::Text("Images");
					ImGui::Separator();
					for (auto& f : aFiles[".dds"])
					{
						if (ImGui::Selectable(f.c_str()))
						{
							data->myTextures[i] = AssetManager::GetInstance().GetTexture(f);
						}
					}
					ImGui::EndPopup();
				}
				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::Separator();
	}
	*/

	float Model::GetSize()
	{
		return myGraphicSize;
	}

	Model::ModelData::~ModelData()
	{
		SAFE_RELEASE(myInputLayout);
		SAFE_RELEASE(myIndexBuffer);
		SAFE_RELEASE(myVertexBuffer);
	}
}