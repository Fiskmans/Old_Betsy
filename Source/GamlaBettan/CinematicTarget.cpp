#include "pch.h"
#include "CinematicTarget.h"
#include "ModelInstance.h"
#include "Model.h"
#ifdef _DEBUG
#include "imgui.h"
#endif

ModelInstance* CinematicTarget::Get()
{
	return myModel;
}

V4F CinematicTarget::GetPosition()
{
	return myOriginalPosition;
}

void CinematicTarget::SetScene(Scene* aScene)
{
	myScene = aScene;
}

void CinematicTarget::Set(ModelInstance* aTarget)
{
	myOriginalPosition = V4F(aTarget->GetPosition(), 1);
	myModel = aTarget;
}

void CinematicTarget::Load(FiskJSON::Object& aObject, Scene* aScene)
{
	myScene = aScene;
	V3F pos;
	std::string path;
	if (aObject["ObjectPos"].GetIf(pos) && aObject["ObjectType"].GetIf(path))
	{
		float closest = float(_HUGE_ENUF);
		ModelInstance* model = nullptr;
		for (auto& i : *aScene)
		{
			if (i->GetModel()->GetModelData()->myFilePath == path)
			{
				float distance = pos.DistanceSqr(i->GetPosition());
				if (distance < closest)
				{
					closest = distance;
					model = i;
				}
			}
		}
		if (model)
		{
			Set(model);
		}
	}
}

void CinematicTarget::AppendTo(FiskJSON::Object& aObject)
{
	aObject.AddValueChild("ObjectType", myModel->GetModel()->GetModelData()->myFilePath);
	aObject.AddValueChild("ObjectPos", V3F(myOriginalPosition));
}

void CinematicTarget::Edit()
{
#ifdef _DEBUG
	static ImGuiID PickingID;
	ImGuiID thisId = ImGui::GetID("Picker");
	bool shouldPopColor = false;
	static std::vector<ModelInstance*> found;
	static std::vector<ModelInstance*> lastFound;

	if (PickingID == thisId)
	{
		shouldPopColor = true;
		ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_BorderShadow]);
		for (auto& i : lastFound)
		{
			i->SetIsHighlighted(false);
		}
		lastFound = myScene->GetIntersections(DebugTools::LastKnownMouseRay);
		for (auto& i : lastFound)
		{
			//i->SetIsHighlighted(true);
		}
		if (GetAsyncKeyState(VK_LBUTTON) && !ImGui::IsPopupOpen("Selector") && !ImGui::GetIO().WantCaptureMouse)
		{
			found = lastFound;
			if (found.size() == 1)
			{
				Set(found[0]);
			}
			if (found.size() > 1)
			{
				ImGui::OpenPopup("Selector");
			}
		};

		if (ImGui::BeginPopup("Selector"))
		{
			ImGui::Text("Models");
			ImGui::Separator();
			for (size_t i = 0; i < found.size(); i++)
			{
				if (ImGui::Selectable(std::to_string(i).c_str()))
				{
					Set(found[i]);
					ImGui::CloseCurrentPopup();
					PickingID = ImGuiID();
					found.clear();
					break;
				}
				found[i]->SetIsHighlighted(ImGui::IsItemHovered());
			}
			ImGui::EndPopup();
		}
	}
	if (ImGui::Button("Select Model"))
	{
		PickingID = thisId;
	}
	if (shouldPopColor)
	{
		ImGui::PopStyleColor(1);
	}
	if (myModel)
	{
		myModel->SetIsHighlighted(ImGui::IsItemHovered());
	}
#endif // _DEBUG
}
