#include "engine/graph/BuiltNode.h"
#include "engine/graph/Node.h"
#include "engine/graph/NodeData.h"

namespace engine::graph
{
	BuiltNode::BuiltNode(NodeBase* aBaseNode)
		: myBaseNode(aBaseNode)
	{
	}

	const char* BuiltNode::Name() 
	{ 
		return typeid(*myBaseNode).name(); 
	}

	void BuiltNode::AddInPin(PinBase* aInPin)
	{
		myInPins.push_back(aInPin);
	}

	void BuiltNode::AddOutPin(PinBase* aOutPin)
	{
		myOutPins.push_back(aOutPin);
	}

	void BuiltNode::AddData(NodeDataBase* aData)
	{
		myData.push_back(aData);
	}

	void BuiltNode::Imgui()
	{
		if (ImGui::TreeNode(Name()))
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

	ImVec2 BuiltNode::ImguiSize(NodeInstanceId aId)
	{
		return myBaseNode->ImguiSize(aId);
	}

	void BuiltNode::Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft)
	{
		myBaseNode->Imgui(aId, aScale, aTopLeft);
	}

	void BuiltNode::AddInstance(NodeInstanceId aId)
	{
		for (PinBase* out : myOutPins)
		{
			out->AddInstance(aId);
			out->GetOutPinInstance(aId)->GetStorage().SetRefreshCallback(std::bind(&NodeBase::Activate, myBaseNode, aId));
		}

		for (PinBase* in : myInPins)
		{
			in->AddInstance(aId);

			InPinInstanceBase* inInstance = in->GetInPinInstance(aId);
			for (PinBase* out : myOutPins)
				inInstance->AddDependent(out->GetOutStorage(aId));
		}

		for (NodeDataBase* data : myData)
			data->AddInsitance(aId);
	}

	void BuiltNode::RemoveInstance(NodeInstanceId aId)
	{
		for (PinBase* in : myInPins)
			in->RemoveInstance(aId);

		for (PinBase* out : myOutPins)
			out->RemoveInstance(aId);

		for (NodeDataBase* data : myData)
			data->RemoveInstance(aId);
	}
}