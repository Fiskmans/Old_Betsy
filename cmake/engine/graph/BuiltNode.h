#ifndef ENGINE_GRAPH_BUILT_NODE_H
#define ENGINE_GRAPH_BUILT_NODE_H

#include "engine/graph/NodePin.h"

#include <string>
#include <typeinfo>


namespace engine::graph
{
	class NodeBase;

	class BuiltNode
	{
	public:
		BuiltNode(NodeBase* aBaseNode);

		const char* Name();

		void AddInPin(PinBase* aInPin);
		void AddOutPin(PinBase* aOutPin);

		void Imgui();

		std::vector<PinBase*>& InPins() { return myInPins; }
		std::vector<PinBase*>& OutPins() { return myOutPins; }

		void AddInstance(NodeInstanceId aId);
		void RemoveInstance(NodeInstanceId aId);

	private:

		NodeBase* myBaseNode;

		std::vector<PinBase*> myInPins;
		std::vector<PinBase*> myOutPins;
	};
}

#endif