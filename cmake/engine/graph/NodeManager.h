#ifndef ENGINE_GRAPH_NODES_MANAGER_H
#define ENGINE_GRAPH_NODES_MANAGER_H

#include "engine/graph/NodePin.h"
#include "engine/graph/BuiltNode.h"
#include "engine/graph/InstancedNodeData.h"

#include "tools/Singleton.h"

#include <vector>
#include <memory>

namespace engine::graph
{
	class Graph;
	class Nodebase;
	class NodeManager : public tools::Singleton<NodeManager>
	{
	public:

		void BeginGroup() { myCurrentGroup = std::make_shared<NodeVarianceGroup>(); };
		void EndGroup() { myCurrentGroup = nullptr; }

		void BeginNode(NodeBase* aBaseNode);
		
		void AddInPin(PinBase* aInPin);
		void AddOutPin(PinBase* aOutPin);
		size_t AddData(InstancedNodeDataBase* aData, size_t aSize, size_t aAligment);

		void EndNode();

		void Imgui();

		template<class NodeType>
		BuiltNode* Get() { return Get(typeid(NodeType).name()); }

		BuiltNode* Get(const std::string& aName);

		static std::string PrettyfyName(const std::string& aName);

		void ImguiAddNodes(Graph* aGraph, float aScale, ImVec2 aPosition);
	private:

		std::shared_ptr<NodeVarianceGroup> myCurrentGroup = nullptr;
		BuiltNode* myCurrent = nullptr;
		std::unordered_map<std::string, BuiltNode*> myNodes;
	};
}

#endif