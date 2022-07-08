#include "engine/graph/BuiltNode.h"

namespace engine::graph
{


	BuiltNode::BuiltNode(const std::string& aName)
		: myName(aName)
	{
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
			//ImGui::Columns(2);
			//for (PinBase* inPin : myInPins)
			//	inPin->Draw();
			//
			//ImGui::NextColumn();
			//for (PinBase* outPin : myOutPins)
			//	outPin->Draw();

			//ImGui::NextColumn();

			ImGui::TreePop();
		}
	}
}