#include "engine/graph/NodeManager.h"
#include "engine/graph/Graph.h"

#include "imgui/WindowControl.h"
#include "imgui/imgui.h"

#include "tools/ImGuiHelpers.h"

#include "logger/Logger.h"


namespace engine::graph
{
	void NodeManager::BeginNode(NodeBase* aBaseNode)
	{
		if (myCurrent)
			EndNode();

		myCurrent = new BuiltNode(aBaseNode);
	}

	void NodeManager::AddInPin(PinBase* aInPin)
	{
		myCurrent->AddInPin(aInPin);
	}

	void NodeManager::AddOutPin(PinBase* aOutPin)
	{
		myCurrent->AddOutPin(aOutPin);
	}

	size_t NodeManager::AddData(InstancedNodeDataBase* aData, size_t aSize, size_t aAligment)
	{
		return myCurrent->AddData(aData, aSize, aAligment);
	}
	
	void NodeManager::EndNode()
	{
		myNodes.emplace(PrettyfyName(myCurrent->Name()), myCurrent);
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

	void NodeManager::ImguiAddNodes(Graph* aGraph, float aScale, ImVec2 aPosition)
	{
		static bool open = false;
		static ImVec2 root;
		
		if (ImGui::GetIO().MouseClicked[1])
		{
			open = true;
			root = ImGui::GetMousePos();
		}

		if (ImGui::GetIO().MouseReleased[1])
			open = false;

		if (open)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			ImVec2 size = ImVec2(150.f, 16.f * myNodes.size());
			drawList->AddRectFilled(root, ImVec2(root.x + size.x, root.y + size.y), tools::GetImColor(ImGuiCol_WindowBg), 2.f);
			drawList->AddRect(root, ImVec2(root.x + size.x, root.y + size.y), tools::GetImColor(ImGuiCol_Border), 2.f);


			ImVec2 pos = root;
			for (std::pair<std::string, BuiltNode*> node : myNodes)
			{
				ImGui::SetCursorScreenPos(pos);
				if (ImGui::Selectable(node.first.c_str(), false, 0, ImVec2(150,16)))
				{
					//ImVec2 topLeft = ImVec2((myPosition.x + aPosition.x) * aScale + offset.x, (myPosition.y + aPosition.y) * aScale + offset.y);

					//(mypos + apos) * aScale + offset = mousepos
					//mousepos - offset = (mypos + apos) * aScale
					//(mousepos - offset) / aScale = mypos + apos
					//(mousepos - offset) / aScale - apos = mypos

					ImVec2 offset = ImGui::GetWindowPos();
					ImVec2 translated = ImVec2((root.x	- offset.x) / aScale - aPosition.x, (root.y - offset.y) / aScale - aPosition.y);
					aGraph->AddNode(*node.second, translated);
				}
				pos.y += 16 + ImGui::GetStyle().FramePadding.y;
			}
		}
		
	}

}