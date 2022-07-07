#include "engine/graph/NodeManager.h"
#include "imgui/WindowControl.h"

#include "imgui/imgui.h"

namespace engine::graph
{
	void NodeManager::BeginNode(const std::string& aName)
	{
		if (myCurrent)
			EndNode();

		myCurrent = new BuiltNode(aName);
	}

	void NodeManager::AddInPin(PinBase* aInPin)
	{
		myCurrent->AddInPin(aInPin);
	}

	void NodeManager::AddOutPin(PinBase* aOutPin)
	{
		myCurrent->AddOutPin(aOutPin);
	}
	
	void NodeManager::EndNode()
	{
		myNodes.push_back(myCurrent);
		myCurrent = nullptr;
	}

	void NodeManager::Imgui()
	{
		old_betsy_imgui::WindowControl::Window("Nodes",
			[&]()
		{
			for (BuiltNode* node : myNodes)
			{
				node->Imgui();
			}
		});
	}

}