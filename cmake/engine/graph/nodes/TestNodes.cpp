#include "engine/graph/nodes/TestNodes.h"

#include <algorithm>

namespace engine::graph::node
{
	void PrintNode::Activate(NodeInstanceId aId)
	{

	}

	ImVec2 PrintNode::ImguiSize(NodeInstanceId aId)
	{
		ImVec2 textSize = ImGui::CalcTextSize(std::to_string(myStored.Get(aId)).c_str());

		ImVec2 out = textSize;
		out.y += 20.f;
		out.x = (std::max)(out.x, 60.f);
		return out;
	}

	void PrintNode::Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::SetCursorScreenPos(aTopLeft);
		if (ImGui::Button("load", ImVec2(60.f, 20.f)))
			myStored.Get(aId) = myIn.Get(aId);

		drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x, aTopLeft.y + 20), ImColor(1.f,1.f,1.f,1.f), std::to_string(myStored.Get(aId)).c_str());
	}

	void IntConstant::Activate(NodeInstanceId aId)
	{
		myOut.Write(aId, myValue.Get(aId));
	}

	ImVec2 IntConstant::ImguiSize(NodeInstanceId aId)
	{
		ImVec2 textSize = ImGui::CalcTextSize(std::to_string(myValue.Get(aId)).c_str());

		ImVec2 out = textSize;
		out.y += 20.f;
		out.x = (std::max)(out.x, 60.f);
		return out;
	}

	void IntConstant::Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::SetCursorScreenPos(aTopLeft);
		if (ImGui::Button("+"))
		{
			myValue.Get(aId)++;
			myOut.Write(aId, myValue.Get(aId));
		}

		ImGui::SameLine();

		if (ImGui::Button("-"))
		{
			myValue.Get(aId)--;
			myOut.Write(aId, myValue.Get(aId));
		}

		drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x, aTopLeft.y + 20), ImColor(1.f, 1.f, 1.f, 1.f), std::to_string(myValue.Get(aId)).c_str());
	}
}
