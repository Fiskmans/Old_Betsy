#include "engine/graph/NodeManager.h"
#include "imgui/WindowControl.h"

#include "logger/Logger.h"

#include "imgui/imgui.h"

namespace engine::graph
{
	void NodeManager::BeginNode(const std::string& aName)
	{
		if (myCurrent)
			EndNode();

		myCurrent = new BuiltNode(PrettyfyName(aName));
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
		myNodes.emplace(myCurrent->Name(), myCurrent);
		myCurrent = nullptr;
	}

	void NodeManager::Imgui()
	{
		old_betsy_imgui::WindowControl::Window("Nodes",
			[&]()
		{
			for (std::pair<std::string,BuiltNode*> node : myNodes)
			{
				node.second->Imgui();
			}
		});
	}

	BuiltNode* NodeManager::Get(const std::string& aName)
	{
		std::string key = PrettyfyName(aName);
		typename decltype(myNodes)::iterator it = myNodes.find(key);
		if (it != myNodes.end())
			return it->second;

		LOG_ERROR("No node with that name exists", aName, key);
		return nullptr;
	}

	std::string NodeManager::PrettyfyName(const std::string& aName)
	{
		std::string name = aName;
		if (name.starts_with("class "))
			name	= name.substr(6);
		else if (name.starts_with("struct "))
			name	= name.substr(7);

		if (name.starts_with("engine::graph::NodeBase<class "))
			name = name.substr(30);

		if (name.ends_with(">"))
			name = name.substr(0, name.size() - 1);

		if (name.ends_with("node") || name.ends_with("Node"))
			name = name.substr(0, name.size() - 4);

		size_t last = name.find_last_of(':');
		if (last != std::string::npos)
			name = name.substr(last + 1);

		return name;
	}

}