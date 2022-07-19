#ifndef ENGINE_GRAPH_NODE_MANAGER_H
#define ENGINE_GRAPH_NODE_MANAGER_H

#include "engine/graph/NodePin.h"
#include "engine/graph/BuiltNode.h"
#include "engine/graph/NodeData.h"

#include "tools/Singleton.h"

#include <vector>

namespace engine::graph
{
	class Graph;
	class Nodebase;
	class NodeManager : public tools::Singleton<NodeManager>
	{
	public:

		void BeginNode(NodeBase* aBaseNode);
		
		void AddInPin(PinBase* aInPin);
		void AddOutPin(PinBase* aOutPin);
		void AddData(NodeDataBase* aData);

		void EndNode();

		void Imgui();

		template<class NodeType>
		BuiltNode* Get() { return Get(typeid(NodeType).name()); }

		BuiltNode* Get(const std::string& aName);

		static std::string PrettyfyName(const std::string& aName);

		void ImguiAddNodes(Graph* aGraph, float aScale, ImVec2 aPosition);
	private:

		BuiltNode* myCurrent = nullptr;
		std::unordered_map<std::string, BuiltNode*> myNodes;
	};
}

#endif