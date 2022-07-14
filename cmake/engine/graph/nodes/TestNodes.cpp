#include "engine/graph/nodes/TestNodes.h"

namespace engine::graph::node
{
	void PrintNode::Activate(NodeInstanceId aId)
	{

	}

	ImVec2 PrintNode::ImguiSize(NodeInstanceId aId)
	{
		return ImGui::CalcTextSize(std::to_string(myIn.Get(aId)).c_str());
	}

	void PrintNode::Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, aTopLeft, ImColor(1.f,1.f,1.f,1.f), std::to_string(myIn.Get(aId)).c_str());
	}
}
