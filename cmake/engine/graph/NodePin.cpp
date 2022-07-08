#include "engine/graph/NodePin.h"
#include "engine/graph/Node.h"
#include "engine/graph/NodeManager.h"

#include "tools/TimeHelper.h"

#include "common/Macros.h"

namespace engine::graph
{
	ImColor ColorFromHashCode(size_t aHashcode)
	{
		float value = aHashcode % 20000;
		return ImColor(0.6f + 0.1f * cos(value), 0.5f + 0.2f * cos(value / 1.8f), 0.5f + 0.3f * cos(value / 3.2f), 1.f);
	}

	void PinBase::Draw()
	{
		const std::type_info& type = Type();
		ImVec2 location	= ImGui::GetCursorScreenPos();

		ImGui::Dummy(ImVec2(22, 16));
		
		static float startHover = 0;
		{
			bool hovered = ImGui::IsItemHovered();
		
			if (hovered && !myIsHovered)
			{
				startHover = tools::GetTotalTime();
			}

			myIsHovered = hovered;
		}

		ImGui::SameLine();
		ImGui::Text("%s", Name());


		ImDrawList * drawList = ImGui::GetWindowDrawList();
		ImColor color = ColorFromHashCode(type.hash_code());

		bool popupOpen = false;

		if (myIsHovered && tools::GetTotalTime() - startHover > 0.4f)
			popupOpen = true;

		if (myIsHovered)
			color = ImColor(LERP(color.Value.x, 1.f, 0.5f), LERP(color.Value.y, 1.f, 0.5f), LERP(color.Value.z, 1.f, 0.5f), 1.f);

		const float yOffset = 7.4f;
		const float xOffset = 6.f;

		const float bigCircleSize = 4.1f;
		const float bigCircleThickness = 3.2f;
		const float lineLength = 3.9f;
		const float lineThickness = 1.4f;
		const float smallCirclesize = 2.8f;
		const float smallCircleThickness = 1.9f;

		//drawList->AddRectFilled(aLocation, ImVec2(aLocation.x + 22, aLocation.y + 16), ImColor(0.5, 0.5, 0.5, 1.f));
		drawList->AddCircle(ImVec2(location.x + xOffset, location.y + yOffset), bigCircleSize, color, 0, bigCircleThickness);
		drawList->AddLine(ImVec2(location.x + xOffset + bigCircleSize, location.y + yOffset), ImVec2(location.x + xOffset + bigCircleSize + lineLength, location.y + yOffset), color, lineThickness);
		drawList->AddCircle(ImVec2(location.x + xOffset + bigCircleSize + lineLength + smallCirclesize, location.y + yOffset), smallCirclesize, color, 0, smallCircleThickness);



		if(popupOpen)
		{
			ImVec2 mousePos = ImGui::GetMousePos();

			ImVec2 textSize = ImGui::CalcTextSize(type.name());

			ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y - textSize.y - ImGui::GetStyle().WindowPadding.y * 2));
			ImGui::SetNextWindowFocus();
			ImGui::Begin("pin_popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
			ImGui::Text("%s", type.name());
			ImGui::End();
		}
	}

	//void PinBase::Setup()
	//{
	//	ImGui::DragFloat("yOffset", &yOffset, 0.1f);
	//	ImGui::DragFloat("xOffset", &xOffset, 0.1f);
	//	ImGui::DragFloat("bigCircleSize", &bigCircleSize, 0.1f);
	//	ImGui::DragFloat("bigCircleThickness", &bigCircleThickness, 0.1f);
	//	ImGui::DragFloat("lineLength", &lineLength, 0.1f);
	//	ImGui::DragFloat("lineThickness", &lineThickness, 0.1f);
	//	ImGui::DragFloat("smallCirclesize", &smallCirclesize, 0.1f);
	//	ImGui::DragFloat("smallCircleThickness", &smallCircleThickness, 0.1f);
	//}

	PinValueBase& InPinInstance::Fetch()
	{
		if (myTarget->IsDirty())
			myRefreshCallback();

		return *myTarget;
	}

	namespace node_pin_helpers
	{
		void RegisterInPin(PinBase* aPin)
		{
			NodeManager::GetInstance().AddInPin(aPin);
		}

		void RegisterOutPin(PinBase* aPin)
		{
			NodeManager::GetInstance().AddOutPin(aPin);
		}
	}
}

