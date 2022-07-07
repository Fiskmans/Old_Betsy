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

	private:
		BuiltNode* myCurrent = nullptr;
		std::vector<BuiltNode*> myNodes;
	};
}

#endif