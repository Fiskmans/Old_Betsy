#include "engine/graph/NodePin.h"
#include "engine/graph/Node.h"
#include "engine/graph/NodeManager.h"
#include "engine/graph/Graph.h"

#include "tools/ImGuiHelpers.h"
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
	const std::type_info* PinBase::ourNextHoveredType = nullptr;
	float PinBase::ourHoverIntensity = 0.f;
	bool PinBase::ourHoverIn = false;

	ImVec2 PinBase::ourHoverTarget;
	bool PinBase::ourIsHoveringTarget = false;
	bool PinBase::ourNextIsHoveringTarget = false;

	bool PinBase::CanConnectTo(const PinBase& aOther) const
	{
		if (Type() != aOther.Type())
			return false; // check if convertible

		if (IsInPin() == aOther.IsInPin())
			return false;

		// check if would create loop

		return true;
	}

	float PinBase::GetImGuiWidth()
	{
		return ImGui::CalcTextSize(Name()).x + 28.f;
	}

	void PinBase::ImGui(Graph* aGraph, float aScale, ImVec2 aLocation, NodeInstanceId aId)
	{
		const bool isIn = IsInPin();

		const float yOffset = 1.4f * aScale;
		const float xOffset = (8.f - (isIn ? 0.f : 26.f)) * aScale;

		const float bigCircleSize = 4.1f * aScale;
		const float bigCircleThickness = 3.2f * aScale;
		const float lineLength = 3.9f * aScale;
		const float lineThickness = 1.4f * aScale;
		const float smallCirclesize = 2.8f * aScale;
		const float smallCircleThickness = 1.9f * aScale;

		static float startHover = 0;

		ImGui::PushID(this);

		const std::type_info& type = Type();

		ImDrawList * drawList = ImGui::GetWindowDrawList();
		ImColor color = ColorFromHashCode(type.hash_code());

		ImVec2 interactableRegionTopLeft = ImVec2(aLocation.x + xOffset - ImGui::GetStyle().FramePadding.x * aScale - 2.f * aScale, aLocation.y - 8.f * aScale);
		ImVec2 interactableRegionSize = ImVec2(22 * aScale, 16 * aScale);

		ImGui::SetCursorScreenPos(interactableRegionTopLeft);
		ImGui::InvisibleButton("drag_source", interactableRegionSize);
		{
			bool hovered = ImGui::IsItemHovered();

			if (hovered && !myIsHovered)
			{
				startHover = tools::GetTotalTime();
			}

			myIsHovered = hovered;
		}

		if (ImGui::BeginDragDropSource())
		{
			PinBase* payload = this;
			ImGui::Text("%s", Name());
			ImGui::Text("%s", type.name());

			ImGui::SetDragDropPayload("NodePin", &payload, sizeof(payload), ImGuiCond_Once);
			ImGui::EndDragDropSource();

			ourNextHoveredType = &type;
			ourHoverIn = isIn;

			const float bendyness = 50.f * aScale * (isIn ? -1.f : 1.f);
			ImVec2 target = ImGui::GetMousePos();
			if (ourIsHoveringTarget)
			{
				target = ourHoverTarget;
			}


			drawList->AddBezierCubic(aLocation, ImVec2(aLocation.x + bendyness, aLocation.y), ImVec2(target.x - bendyness, target.y), target, ImColor(0.8f, 0.8f, 0.8f, 0.8f), 2.f * aScale);
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodePin", ImGuiDragDropFlags_AcceptPeekOnly | ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
			if (payload)
			{
				PinBase& source = **reinterpret_cast<PinBase**>(payload->Data);
				
				if (CanConnectTo(source))
				{
					if (payload->IsPreview())
					{
						drawList->AddRect(interactableRegionTopLeft, ImVec2(interactableRegionTopLeft.x + interactableRegionSize.x, interactableRegionTopLeft.y + interactableRegionSize.y), tools::GetImColor(ImGuiCol_Border), 3.f, 0, 2.f * aScale);

						ourNextIsHoveringTarget = true;
						ourHoverTarget = aLocation;
					}

					if(payload->IsDelivery())
					{
						InPinInstance* in = nullptr;
						OutPinInstanceBase* out = nullptr;

						if (isIn)
						{
							in = this->GetInPinInstance(aId);
							out = source.GetOutPinInstance(aId);
						}
						else
						{
							in = source.GetInPinInstance(aId);
							out = this->GetOutPinInstance(aId);
						}

						aGraph->AddLink(out, in);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}


		bool popupOpen = false;

		const float timeToPopUp = 0.4f;
		
		if (myIsHovered)
		{
			const float hoverIntensity = 0.5f;

			if (tools::GetTotalTime() - startHover > timeToPopUp)
				popupOpen = true;

			color = ImColor(LERP(color.Value.x, 1.f, hoverIntensity), LERP(color.Value.y, 1.f, hoverIntensity), LERP(color.Value.z, 1.f, hoverIntensity), 1.f);
			ourNextHoveredType = &type;
			ourHoverIn = isIn;
		}

		if (ourHoveredType && *ourHoveredType == type && ourHoverIn != isIn)
		{
			float intensity = (0.3f + 0.3f * cos(tools::GetTotalTime() * TAU)) * ourHoverIntensity;
			color = ImColor(LERP(color.Value.x, 1.f, intensity), LERP(color.Value.y, 1.f, intensity), LERP(color.Value.z, 1.f, intensity), 1.f);
		}

		//drawList->AddRectFilled(aLocation, ImVec2(aLocation.x + 22, aLocation.y + 16), ImColor(0.5, 0.5, 0.5, 1.f));
		drawList->AddCircle(ImVec2(aLocation.x + xOffset, aLocation.y + yOffset), bigCircleSize, color, 0, bigCircleThickness);
		drawList->AddLine(ImVec2(aLocation.x + xOffset + bigCircleSize, aLocation.y + yOffset), ImVec2(aLocation.x + xOffset + bigCircleSize + lineLength, aLocation.y + yOffset), color, lineThickness);
		drawList->AddCircle(ImVec2(aLocation.x + xOffset + bigCircleSize + lineLength + smallCirclesize, aLocation.y + yOffset), smallCirclesize, color, 0, smallCircleThickness);

		float textOffset = 26.f;

		if (!isIn)
		{
			ImVec2 textSize = ImGui::CalcTextSize(Name());
			textOffset = -textSize.x - 26.f;
		}

		drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aLocation.x + textOffset * aScale, aLocation.y - 5.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), Name());

		if(popupOpen)
		{
			ImVec2 mousePos = ImGui::GetMousePos();

			ImVec2 textSize = ImGui::CalcTextSize(type.name());
			float popupPadding = 4;

			ImVec2 popupTopLeft = ImVec2(mousePos.x + 4, mousePos.y - textSize.y - 4 - popupPadding * 2);
			ImVec2 popupBottomRight = ImVec2(popupTopLeft.x + textSize.x + popupPadding * 2, mousePos.y - 4);

			ImVec2 textOffset = ImVec2(popupPadding, popupPadding);

			drawList->AddRectFilled(popupTopLeft, popupBottomRight, tools::GetImColor(ImGuiCol_PopupBg), ImGui::GetStyle().PopupRounding);
			drawList->AddRect(popupTopLeft, popupBottomRight, tools::GetImColor(ImGuiCol_Border), ImGui::GetStyle().PopupRounding);
			drawList->AddText(ImVec2(popupTopLeft.x + textOffset.x, popupTopLeft.y + textOffset.x), ImColor(1.f,1.f,1.f,1.f), type.name());
		}

		ImGui::PopID();
	}

	void PinBase::Imgui()
	{
		static float lastTick = tools::GetTotalTime();
		static float sum = 0;

		float now = tools::GetTotalTime();

		ourHoveredType = ourNextHoveredType;
		ourNextHoveredType = nullptr;

		ourIsHoveringTarget = ourNextIsHoveringTarget;
		ourNextIsHoveringTarget = false;

		if (ourHoveredType)
			sum += now - lastTick;
		else
			sum = 0;

		const float timeToHighlight = 1.1f;

		if (sum > timeToHighlight)
		{
			float x = sum - timeToHighlight;
			ourHoverIntensity = x / sqrt(1 + tools::Square(x));
		}
		else
		{
			ourHoverIntensity = 0;
		}

		lastTick = now;
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
		myTarget->Load();
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

