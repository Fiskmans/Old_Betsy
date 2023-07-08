#include "imgui/CustomWidgets.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace old_betsy_imgui
{
	void FloatIndicator(float aValue)
	{
		using namespace ImGui;

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiStyle& style = g.Style;
		ImVec2 size = CalcItemSize(ImVec2(100, 10), CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
		ImVec2 pos = window->DC.CursorPos;
		ImRect bb(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
		ItemSize(size);
		if (!ItemAdd(bb, 0))
			return;

		float t0 = 0.5f;
		float t1 = 0.5f + 0.5f * aValue;

		if (t1 < 0.f)
		{
			t1 = 0.f;
		}

		if (t1 > 1.f)
		{
			t1 = 1.f;
		}

		RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
		bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
		RenderRectFilledRangeH(window->DrawList, bb, GetColorU32(ImGuiCol_PlotHistogram), t0, t1, style.FrameRounding);
		window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), style.FrameRounding);
		window->DrawList->AddLine(ImVec2(pos.x + 0.5f * size.x, pos.y), ImVec2(pos.x + 0.5f * size.x, pos.y + size.y), GetColorU32(ImGuiCol_Border));
	}
}