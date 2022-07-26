#include "engine/graph/DrawablePinBlock.h"

#include "engine/graph/NodePin.h"
#include "engine/graph/Graph.h"

namespace engine::graph
{
	DrawablePinBlock::DrawablePinBlock(ImVec2 aPosition)
		: myPosition(aPosition)
	{
	}

	bool DrawablePinBlock::Imgui(const char* aName, Graph* aGraph, float aScale, ImVec2 aPosition, const std::vector<PinBase*>& aInPins, const std::vector<PinBase*>& aOutPins)
	{
		ImGui::PushID(this);

		ImVec2 offset = ImGui::GetWindowPos();
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		const ImVec2 shadowOffset = ImVec2(6 * aScale, 6 * aScale);


		const ImVec2 HeaderTextSize = ImGui::CalcTextSize(aName);
		const float		headerSize = 20 * aScale;
		const float		pinSpacing = 4 * aScale;

		ImVec2 leftPinBlockSize = ImVec2(pinSpacing, 0);
		for (PinBase* pin : aInPins)
		{
			ImVec2 size = pin->ImGuiSize();
			leftPinBlockSize.y += size.y * aScale + pinSpacing;
			leftPinBlockSize.x = (std::max)(size.x * aScale, leftPinBlockSize.x);
		}

		ImVec2 rightPinBlockSize = ImVec2(pinSpacing, 0);
		for (PinBase* pin : aOutPins)
		{
			ImVec2 size = pin->ImGuiSize();
			rightPinBlockSize.y += size.y * aScale + pinSpacing;
			rightPinBlockSize.x = (std::max)(size.x * aScale, rightPinBlockSize.x);
		}


		ImVec2 rawCustomSize = CustomImguiSize();
		ImVec2 customSize = ImVec2(rawCustomSize.x * aScale, rawCustomSize.y * aScale);

		const float separatorSize = 7.f * aScale;
		const float contentHeight = (std::max)( customSize.y, (std::max)(leftPinBlockSize.y, rightPinBlockSize.y) );


		ImVec2 size = ImVec2(separatorSize * 2.f + (std::max)(leftPinBlockSize.x + rightPinBlockSize.x + customSize.x, (HeaderTextSize.x + 8.f) * aScale), headerSize + contentHeight + 2.f * aScale);
		ImVec2 topLeft = ImVec2((myPosition.x + aPosition.x) * aScale + offset.x, (myPosition.y + aPosition.y) * aScale + offset.y);
		ImVec2 bottomRight = ImVec2(topLeft.x + size.x, topLeft.y + size.y);

		Selection& selection = aGraph->GetSelection();

		if (selection.IsSelecting())
		{
			if (selection.Intersects(topLeft, bottomRight))
			{
				selection.AddToSelection(this);
			}
		}

		ImGui::SetCursorScreenPos(topLeft);
		ImGui::InvisibleButton("heading", ImVec2(size.x, headerSize));

		bool headerHovered = ImGui::IsItemHovered();
		bool headerActive = selection.IsSelected(this);

		ImGuiIO& io = ImGui::GetIO();
		if (headerHovered && io.MouseClicked[0] && !io.KeysDown[VK_SPACE])
			myIsMoving = true;

		if (io.MouseReleased[0])
		{
			if (myIsMoving)
				selection.InvalidateIfOnly(this);

			myIsMoving = false;
		}

		if (myIsMoving)
		{
			ImVec2 delta = ImVec2(io.MouseDelta.x / aScale, io.MouseDelta.y / aScale);
			if (!selection.IsSelected(this))
			{
				selection.InvalidateSelection();
				selection.AddToSelection(this);
				selection.UpdateSelection();
			}

			selection.MoveAll(delta);
		}


		drawlist->AddRectFilled(ImVec2(topLeft.x + shadowOffset.x, topLeft.y + shadowOffset.y), ImVec2(bottomRight.x + shadowOffset.x, bottomRight.y + shadowOffset.y), tools::GetImColor(ImGuiCol_BorderShadow), 3.f);
		drawlist->AddRectFilled(topLeft, bottomRight, tools::GetImColor(ImGuiCol_WindowBg), 3.f);
		drawlist->AddRectFilled(topLeft, ImVec2(bottomRight.x, topLeft.y + headerSize), tools::GetImColor(headerActive ? ImGuiCol_HeaderActive : (headerHovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header)), 3.f);
		drawlist->AddRect(topLeft, bottomRight, tools::GetImColor(ImGuiCol_Border), 3.f);


		ImVec2 textPos = ImVec2(topLeft.x + 4 * aScale, topLeft.y + 4 * aScale);

		drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, textPos, tools::GetImColor(ImGuiCol_Text), aName);

		bool interacting = false;
		{
			ImVec2 pinPosition = ImVec2(topLeft.x, topLeft.y + headerSize + pinSpacing);

			for (PinBase* pin : aInPins)
			{
				ImVec2 attachPoint;
				interacting |= pin->ImGui(aGraph, aScale, pinPosition, attachPoint);
				aGraph->AddPinLocation(pin->GetInPinInstance(), attachPoint);

				pinPosition.y += pinSpacing + pin->ImGuiSize().y * aScale;
			}
		}

		{
			ImVec2 pinPosition = ImVec2(bottomRight.x, topLeft.y + headerSize + pinSpacing);

			for (PinBase* pin : aOutPins)
			{
				ImVec2 attachPoint;
				interacting |= pin->ImGui(aGraph, aScale, pinPosition, attachPoint);
				aGraph->AddPinLocation(pin->GetOutPinInstance(), attachPoint);

				pinPosition.y += pinSpacing + pin->ImGuiSize().y * aScale;
			}
		}

		ImVec2 customLocation = ImVec2(topLeft.x + leftPinBlockSize.x + separatorSize, topLeft.y + headerSize);

		CustomImgui(aScale, customLocation);


		ImGui::PopID();
		return myIsMoving || interacting;
	}

	void DrawablePinBlock::Move(ImVec2 aDelta)
	{
		myPosition.x += aDelta.x;
		myPosition.y += aDelta.y;
	}
}