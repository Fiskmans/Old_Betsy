#ifndef ENGINE_GRAPH_GRAPH_H
#define ENGINE_GRAPH_GRAPH_H

#include "tools/Singleton.h"

#include "engine/graph/NodeManager.h"
#include "engine/graph/BuiltNode.h"

#include "imgui/imgui.h"

#include <vector>
#include <memory>
#include <string>

namespace engine::graph
{
	class NodeInstance
	{
	public:
		NodeInstance(BuiltNode& aType, ImVec2 aPosition);

		bool Imgui(float aScale, ImVec2 aPosition);
	private:
		BuiltNode* myType;
		ImVec2 myPosition;
	};

	struct GraphExportPin
	{
		PinBase* myPin = nullptr;
		ImVec2 myPosition;
	};


	class Graph
	{
	public:
		Graph(const std::string& aName);
		~Graph();

		template<class NodeType>
		void AddNode(ImVec2 aPosition) 
		{ 
			BuiltNode* nodeType = NodeManager::GetInstance().Get<NodeType>();
			if (!nodeType)
				return;

			AddNode(*nodeType, aPosition);
		}

		void AddNode(BuiltNode& aType, ImVec2 aPosition);

		bool Imgui(float aScale, ImVec2 aPosition);
		const std::string& Name();
	private:

		std::string myName;
		std::vector<std::unique_ptr<NodeInstance>> myNodes;
	};


	class GraphManager : public tools::Singleton<GraphManager>
	{
	public:
		void AddGraph(Graph* aGraph);
		void RemoveGraph(Graph* aGraph);

		void Imgui();

		bool ManageGraphs();

	private:

		std::vector<Graph*> myGraphs;
	};
}

#endif