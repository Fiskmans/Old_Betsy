#ifndef ENGINE_GRAPH_NODES_MUL_NODE_H
#define ENGINE_GRAPH_NODES_MUL_NODE_H

#include "engine/graph/Node.h"

namespace engine::graph::node
{
	template<typename T>
	class MulNode : public NodeBase
	{
	public:
		void Activate() override
		{
			myResult = myA * myB;
		}

	private:
		InPin<T> myA = PinInformation("A");
		InPin<T> myB = PinInformation("B");

		OutPin<T> myResult = PinInformation("Result");
	};
}

#endif

