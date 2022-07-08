#include "engine/graph/NodePin.h"
#include "engine/graph/Node.h"
#include "engine/graph/NodeManager.h"

#include "tools/TimeHelper.h"
#include "tools/Utility.h"

#include "common/Macros.h"

namespace engine::graph
{
	ImColor ColorFromHashCode(size_t aHashcode)
	{
		float value = aHashcode % 20000;
		return ImColor(
			0.6f + 0.1f * cos(value), 
			0.5f + 0.2f * cos(value / 1.8f), 
			0.5f + 0.3f * cos(value / 3.2f), 
			1.f);
	}

	const std::type_info* PinBase::ourHoveredType = nullptr;
	float PinBase::ourHoverIntensity = 0.f;

	void PinBase::Draw(float aScale, ImVec2 aLocation)
	{
		const std::type_info& type = Type();

		ImGui::SetCursorScreenPos(aLocation);
		ImGui::Dummy(ImVec2(22 * aScale, 16 * aScale));
		
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
		bool highlight = false;

		const float timeToPopUp = 0.4f;
		const float timeToHighlight = 1.1f;
		
		if (myIsHovered)
		{
			const float hoverIntensity = 0.5f;

			if (tools::GetTotalTime() - startHover > timeToPopUp)
				popupOpen = true;


			if (tools::GetTotalTime() - startHover > timeToHighlight)
				highlight = true;

			color = ImColor(LERP(color.Value.x, 1.f, hoverIntensity), LERP(color.Value.y, 1.f, hoverIntensity), LERP(color.Value.z, 1.f, hoverIntensity), 1.f);
		}

		if(highlight)
		{
			float x				= tools::GetTotalTime() - startHover - timeToHighlight;
			ourHoverIntensity	= x / sqrt(1 + tools::Square(x));
			ourHoveredType		= &type;
		}

		if (ourHoveredType && *ourHoveredType == type)
		{
			float intensity = (0.3f + 0.3f * cos(tools::GetTotalTime() * TAU)) * ourHoverIntensity;
			color = ImColor(LERP(color.Value.x, 1.f, intensity), LERP(color.Value.y, 1.f, intensity), LERP(color.Value.z, 1.f, intensity), 1.f);
		}

		const float yOffset = 7.4f * aScale;
		const float xOffset = 6.f * aScale;

		const float bigCircleSize = 4.1f * aScale;
		const float bigCircleThickness = 3.2f * aScale;
		const float lineLength = 3.9f * aScale;
		const float lineThickness = 1.4f * aScale;
		const float smallCirclesize = 2.8f * aScale;
		const float smallCircleThickness = 1.9f * aScale;

		//drawList->AddRectFilled(aLocation, ImVec2(aLocation.x + 22, aLocation.y + 16), ImColor(0.5, 0.5, 0.5, 1.f));
		drawList->AddCircle(ImVec2(aLocation.x + xOffset, aLocation.y + yOffset), bigCircleSize, color, 0, bigCircleThickness);
		drawList->AddLine(ImVec2(aLocation.x + xOffset + bigCircleSize, aLocation.y + yOffset), ImVec2(aLocation.x + xOffset + bigCircleSize + lineLength, aLocation.y + yOffset), color, lineThickness);
		drawList->AddCircle(ImVec2(aLocation.x + xOffset + bigCircleSize + lineLength + smallCirclesize, aLocation.y + yOffset), smallCirclesize, color, 0, smallCircleThickness);



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

