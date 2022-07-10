#ifndef ENGINE_GRAPH_NODE_TEST_NODES_H
#define ENGINE_GRAPH_NODE_TEST_NODES_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

namespace engine::graph::node 
{
	class PrintNode : public NodeBase
	{
	public:
		void Activate(NodeInstanceId aId) override;

		//ImVec2 ImguiSize(NodeInstanceId aId) override;
		//void Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft) override;
	private:
		InPin<int> myIn = "Val";
	};
}

#endif