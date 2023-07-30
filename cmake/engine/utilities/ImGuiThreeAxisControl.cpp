
#include "engine/utilities/ImGuiThreeAxisControl.h"

#include "imgui/imgui_internal.h"

namespace engine::utilities
{
	tools::V3f ImguiThreeAxisControl(const char* aId, ImVec2 aSize)
	{
		ImGui::PushID(aId);
		ImGuiStyle& style = ImGui::GetStyle();

		ImVec2 size = aSize;

		if (size.x == 0.f)
		{
			size.x = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - style.WindowPadding.x * 2 - style.FramePadding.x;
		}

		if (size.y == 0.f)
		{
			size.y = size.x * 0.5f;
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 bottomRight = ImVec2(pos.x + size.x, pos.y + size.y);

		drawList->AddRect(pos, bottomRight, ImColor(255,0,0));

		ImGui::ItemSize(size);

		ImGui::PopID();

		return tools::V3f();
	}
}
