#include "engine/graph/nodes/TestNodes.h"

#include <algorithm>

namespace engine::graph::nodes
{
	void PrintNode::Activate()
	{

	}

	ImVec2 PrintNode::ImguiSize()
	{
		ImVec2 textSize = ImGui::CalcTextSize(std::to_string(myStored.Get()).c_str());

		ImVec2 out = textSize;
		out.y += 20.f;
		out.x = (std::max)(out.x, 60.f);
		return out;
	}

	void PrintNode::Imgui(float aScale, ImVec2 aTopLeft)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::SetCursorScreenPos(aTopLeft);
		if (ImGui::Button("load", ImVec2(60.f, 20.f)))
			myStored = myIn.Get();

		drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x, aTopLeft.y + 20), ImColor(1.f,1.f,1.f,1.f), std::to_string(myStored.Get()).c_str());
	}

	void IntConstant::Activate()
	{
		myOut = myValue.Get();
	}

	ImVec2 IntConstant::ImguiSize()
	{
		ImVec2 textSize = ImGui::CalcTextSize(std::to_string(myValue.Get()).c_str());

		ImVec2 out = textSize;
		out.y += 20.f;
		out.x = (std::max)(out.x, 60.f);
		return out;
	}

	void IntConstant::Imgui(float aScale, ImVec2 aTopLeft)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::SetCursorScreenPos(aTopLeft);
		if (ImGui::Button("+"))
		{
			myValue.Get()++;
			myOut = myValue.Get();
		}

		ImGui::SameLine();

		if (ImGui::Button("-"))
		{
			myValue.Get()--;
			myOut = myValue.Get();
		}

		drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x, aTopLeft.y + 20), ImColor(1.f, 1.f, 1.f, 1.f), std::to_string(myValue.Get()).c_str());
	}

	void EngineTextureNode::Activate()
	{
		const char* options[] =
		{
			"engine/error.dds",
			"engine/SSAONormal.dds",
			"engine/Vignette.dds",
			"engine/NoImage.dds"
		};

		int index = mySelection.Get();

		if (index < 0 || index >= (sizeof(options) / sizeof(*options)))
		{
			myPath = "";
			return;
		}

		myPath = options[index];
	}
}
