#ifndef ENGINE_GRAPH_NODES_MUL_NODE_H
#define ENGINE_GRAPH_NODES_MUL_NODE_H

#include "engine/graph/Node.h"

namespace engine::graph::node
{
	class MulNode : public NodeBase
	{
	public:
		void Activate() override
		{
			myResult = myA * myB;
		}

	private:
		InPin<int> myA = PinInformation("A");
		InPin<int> myB = PinInformation("B");

		OutPin<int> myResult = PinInformation("Result");
	};
}

#endif

