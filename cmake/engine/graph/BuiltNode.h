#ifndef ENGINE_GRAPH_BUILT_NODE_H
#define ENGINE_GRAPH_BUILT_NODE_H

#include "engine/graph/NodePin.h"

#include <string>
#include <typeinfo>


namespace engine::graph
{
	class NodeBase;
	class NodeDataBase;

	class BuiltNode
	{
	public:
		BuiltNode(NodeBase* aBaseNode);

		const char* Name();

		void AddInPin(PinBase* aInPin);
		void AddOutPin(PinBase* aOutPin);
		void AddData(NodeDataBase* aData);

		void Imgui();

		ImVec2 ImguiSize(NodeInstanceId aId);
		void Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft);
		
		std::vector<PinBase*>& InPins() { return myInPins; }
		std::vector<PinBase*>& OutPins() { return myOutPins; }

		void AddInstance(NodeInstanceId aId);
		void RemoveInstance(NodeInstanceId aId);

	private:

		NodeBase* myBaseNode;

		std::vector<PinBase*> myInPins;
		std::vector<PinBase*> myOutPins;
		std::vector<NodeDataBase*> myData;
	};
}

#endif