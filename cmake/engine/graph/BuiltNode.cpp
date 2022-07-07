#include "engine/graph/BuiltNode.h"

namespace engine::graph
{


	BuiltNode::BuiltNode(const std::string& aName)
		: myName(aName)
	{
		if (myName.starts_with("class "))
			myName = myName.substr(6);
		else if (myName.starts_with("struct "))
			myName = myName.substr(7);

		if (myName.starts_with("engine::graph::NodeBase<class "))
			myName = myName.substr(30);

		if (myName.ends_with(">"))
			myName = myName.substr(0, myName.size() - 1);

		if (myName.ends_with("node") || myName.ends_with("Node"))
			myName = myName.substr(0, myName.size() - 4);

		size_t last = myName.find_last_of(':');
		if (last != std::string::npos)
			myName = myName.substr(last + 1);

		if (myName.empty())
			throw std::exception("Node created with no name");
	}

	void BuiltNode::AddInPin(PinBase* aInPin)
	{
		myInPins.push_back(aInPin);
	}

	void BuiltNode::AddOutPin(PinBase* aOutPin)
	{
		myOutPins.push_back(aOutPin);
	}

	void BuiltNode::Imgui()
	{
		if (ImGui::TreeNode(myName.c_str()))
		{
			ImGui::Columns(2);
			for (PinBase* inPin : myInPins)
			{
				ImVec2 location = ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(22, 16));
				inPin->Draw(location, ImGui::IsItemHovered());
				ImGui::SameLine();
				ImGui::Text("%s", inPin->Name());
			}

			ImGui::NextColumn();
			for (PinBase* outPin : myOutPins)
			{
				ImVec2 location = ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(22, 16));
				outPin->Draw(location, ImGui::IsItemHovered());
				ImGui::SameLine();
				ImGui::Text("%s", outPin->Name());
			}

			ImGui::NextColumn();

			ImGui::TreePop();
		}
	}
}