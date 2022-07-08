#ifndef ENGINE_GRAPH_NODE_MANAGER_H
#define ENGINE_GRAPH_NODE_MANAGER_H

#include "engine/graph/NodePin.h"
#include "engine/graph/BuiltNode.h"

#include "tools/Singleton.h"

#include <vector>

namespace engine::graph
{
	class NodeManager : public tools::Singleton<NodeManager>
	{
	public:

		void BeginNode(const std::string& aName);
		void AddInPin(PinBase* aInPin);
		void AddOutPin(PinBase* aOutPin);
		void EndNode();

		void Imgui();

		template<class NodeType>
		BuiltNode* Get() { return Get(typeid(NodeType).name()); }

		BuiltNode* Get(const std::string& aName);

	private:
		static std::string PrettyfyName(const std::string& aName);

		BuiltNode* myCurrent = nullptr;
		std::unordered_map<std::string, BuiltNode*> myNodes;
	};
}

#endif